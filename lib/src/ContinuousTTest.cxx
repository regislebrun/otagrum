//                                               -*- C++ -*-
/**
 *  @brief ContinuousTTest
 *
 *  Copyright 2010-2018 Airbus-LIP6-Phimeca
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cmath>
#include <tuple>

#include <openturns/BernsteinCopulaFactory.hxx>
#include <openturns/EmpiricalBernsteinCopula.hxx>
#include <openturns/NormalCopulaFactory.hxx>
#include <openturns/SpecFunc.hxx>
#include <openturns/DistFunc.hxx>

#include "otagr/ContinuousTTest.hxx"

constexpr double pi() { return std::atan(1) * 4; }

namespace OTAGR {
double ContinuousTTest::z2p_(const double z) {
  return OT::DistFunc::pNormal(-z);
}

double ContinuousTTest::z2p_interval_(const double z) {
  return OT::DistFunc::pNormal(z) - OT::DistFunc::pNormal(-z);
}

OT::UnsignedInteger ContinuousTTest::getK_(const OT::Sample & sample) {
  return OT::UnsignedInteger(1.0 + std::pow(sample.getSize(), 2.0 / (4.0 + sample.getDimension())));
}

std::string ContinuousTTest::getKey_(const OT::Indices & l,
                                     const OT::UnsignedInteger k) {
  //@todo : sorting l before computing the key ?
  return l.__str__() + ":" + std::to_string(k);
}

ContinuousTTest::ContinuousTTest(const OT::Sample & data,
				 const double alpha) {
  setAlpha(alpha);
  data_ = (data.rank() + 1.0) / data.getSize();
}

OT::Point ContinuousTTest::getPDF_(const OT::Indices & l,
                                   OT::UnsignedInteger k) const {
  if (l.getSize() == 0) {
    //@todo Is this correct ?
    return OT::Point(1, 1 / data_.getSize());
  }
  // default value for k only depends on the size of the sample
  if (k == 0) {
    // k =BernsteinCopulaFactory::ComputeLogLikelihoodBinNumber(sample,2);
    // k =BernsteinCopulaFactory::ComputeAMISEBinNumber(sample,2);
    k = getK_(data_);
  }

  const auto key = getKey_(l, k);
  if (cache_.exists(key)) {
    return cache_.get(key);
  }
  auto dL = data_.getMarginal(l);

  // OT::BernsteinCopulaFactory factory;
  // auto pdf = factory.build(dL, k).computePDF(dL).asPoint();

  // OT::NormalCopulaFactory factory;
  // auto pdf = factory.build(dL).computePDF(dL).asPoint();

  auto pdf = OT::EmpiricalBernsteinCopula(dL, k, true).computePDF(dL).asPoint();

  cache_.set(l.getSize(), key, pdf);
  return pdf;
}

std::tuple<OT::Point, OT::Point, OT::Point, OT::Point, OT::UnsignedInteger>
ContinuousTTest::getPDFs_(const Indice Y, const Indice Z, const OT::Indices & X) const {
  //@todo how to be smart for k ?
  OT::UnsignedInteger k = getK_(data_);
  OT::Point pdf1(getPDF_(X, k));
  OT::Point pdf2(getPDF_(X + Y, k));
  OT::Point pdf3(getPDF_(X + Z, k));
  OT::Point pdf4(getPDF_(X + Y + Z, k));
  return std::make_tuple(pdf1, pdf2, pdf3, pdf4, k);
}

void ContinuousTTest::setAlpha(const double alpha) { alpha_ = alpha; }

double ContinuousTTest::getAlpha() const { return alpha_; }

inline double pPar1MoinsP(const double p) { return p * (1 - p); }

double ContinuousTTest::getTTest(const Indice Y, const Indice Z,
                                 const OT::Indices & X) const {
  OT::UnsignedInteger k;

  auto dY = data_.getMarginal(Y);
  auto dZ = data_.getMarginal(Z);
  auto dX = data_.getMarginal(X);

  OT::Point fX, fYX, fZX, fYZX;
  std::tie(fX, fYX, fZX, fYZX, k) = getPDFs_(Y, Z, X);

  auto d = double(X.getSize()); // such that d/2 is double as well
  auto N = data_.getSize();

  auto C1 = std::pow(2, -(d + 2)) * std::pow(pi(), (d + 2) / 2);
  auto sigma = std::sqrt(2) * std::pow(pi() / 4, (d + 2) / 2);

  double H = 0;
  double B1 = 0;
  double B2 = 0;
  double B3 = 0;
  const double facteurpi = std::pow(4 * pi(), -(d + 1) / 2);
  double ratioH;
  for (unsigned int i = 0; i < N; ++i) {
    ratioH = fYZX[i];
    if (X.getSize() >= 1) {
      ratioH *= fX[i];
    }
    //@todo anything else for 1e-10 (or even small value)
    if (ratioH > 1e-20) {
      H += std::pow(1 - std::sqrt(fYX[i] * fZX[i] / ratioH), 2);
      double vfx;
      double gX = 1.0;
      if (d != 0) {
        for (int j = 0; j < d; ++j) {
          gX /= pPar1MoinsP(dX(i, j));
        }
        gX = std::sqrt(gX);
        vfx = fX[i];
      } else {
        vfx = fX[0];
      }
      B1 += facteurpi * gX / (std::sqrt(pPar1MoinsP(dY(i, 0))) * fYX[i]);
      B2 += facteurpi * gX / (std::sqrt(pPar1MoinsP(dZ(i, 0))) * fZX[i]);
      B3 += vfx * gX;
    }
  }
  // mean
  H /= N;

  const double term12 = -std::pow(2, -d) * std::pow(pi(), (d + 1) / 2);
  const double fact3 = std::pow(2, -(d + 1)) * std::pow(pi(), -d / 2);
  B1 = term12 + B1 / N;
  B2 = term12 + B2 / N;
  B3 = fact3 * B3 / N;

  auto T = std::pow(k, -(d + 2) / 2) / sigma;
  T *= 4 * H * N - pow(k, d / 2) * (C1 * k + (B1 + B2) * std::sqrt(k) + B3);

  return T;
}

double ContinuousTTest::getTTestWithoutCorrections(Indice Y, Indice Z,
                                                   const OT::Indices &X) const {
  OT::UnsignedInteger k;

  auto dY = data_.getMarginal(Y);
  auto dZ = data_.getMarginal(Z);
  auto dX = data_.getMarginal(X);

  OT::Point fX, fYX, fZX, fYZX;
  std::tie(fX, fYX, fZX, fYZX, k) = getPDFs_(Y, Z, X);

  auto d = double(X.getSize()); // such that d/2 is double as well
  auto N = data_.getSize();

  auto sigma = std::sqrt(2) * std::pow(pi() / 4, (d + 2) / 2);

  double H = 0;

  for (unsigned int i = 0; i < N; ++i) {
    double ratioH = fYZX[i];
    if (X.getSize() > 1) {
      ratioH *= fX[i];
    }
    //@todo anything else for 1e-10 (or even small value)
    // if (ratioH > 1e-10) {
    H += std::pow(1 - std::sqrt(fYX[i] * fZX[i] / ratioH), 2);

    //}
  }

  auto T = 4 * H * N - pow(k, d / 2) * std::pow(k, -(d + 2) / 2) / sigma;
  return T;
}

std::tuple<double, double, bool>
ContinuousTTest::isIndep(const Indice Y, const Indice Z, const OT::Indices & X) const {
  return ContinuousTTest::isIndepFromTest(getTTest(Y, Z, X), alpha_);
}

std::tuple<double, double, bool>
ContinuousTTest::isIndepFromTest(const double t, const double alpha) {
  double p = z2p_interval_(t);
  return std::make_tuple(t, p, p >= alpha);
}

std::string ContinuousTTest::__str__(const std::string &offset) const {
  std::stringstream ss;
  ss << offset << "Data dimension : " << data_.getDimension() << std::endl;
  ss << offset << "Data size : " << data_.getSize() << std::endl;
  ss << offset << "Cache : " << std::endl
     << cache_.__str__(offset + "      |") << std::endl;
  ss << offset << "alpha :" << getAlpha() << std::endl;
  return ss.str();
}

void ContinuousTTest::clearCache() const { cache_.clear(); }

void ContinuousTTest::clearCacheLevel(const OT::UnsignedInteger level) const {
  cache_.clearLevel(level);
}

OT::UnsignedInteger ContinuousTTest::getDimension() const {
  return data_.getDimension();
}
} // namespace OTAGR