//                                               -*- C++ -*-
/**
 *  @brief ContinuousPC
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

#ifndef OTAGRUM_CONTINUOUSPC_HXX
#define OTAGRUM_CONTINUOUSPC_HXX

#include "otagrum/ContinuousTTest.hxx"
#include <agrum/graphs/mixedGraph.h>
#include <agrum/graphs/undiGraph.h>
#include <openturns/Sample.hxx>

namespace OTAGRUM {
class ContinuousPC {
public:
  explicit ContinuousPC(const OT::Sample &data,
                        OT::UnsignedInteger maxParents = 5, double alpha = 0.1);

  gum::UndiGraph getSkeleton();
  gum::MixedGraph getPDAG(const gum::UndiGraph &g);
  gum::UndiGraph getMoralGraph(const gum::MixedGraph &g) const;

  void setOptimalPolicy(bool policy) { optimalPolicy_ = policy; };
  bool getOptimalPolicy() const { return optimalPolicy_; };

  void setVerbosity(bool verbose) { verbose_ = verbose; };
  bool getVerbosity() const { return verbose_; };

  double getPValue(gum::NodeId x, gum::NodeId y) {
    gum::Edge e(x, y);
    if (pvalues_.exists(e)) {
      return pvalues_[e];
    } else {
      throw OT::InvalidArgumentException(HERE)
          << "Error: No p-value for edge (" << e.first() << "," << e.second()
          << ").";
    }
  }
  double getTTest(gum::NodeId x, gum::NodeId y) {
    gum::Edge e(x, y);
    if (ttests_.exists(e)) {
      return ttests_[e];
    } else {
      throw OT::InvalidArgumentException(HERE)
          << "Error: No ttest value for edge (" << e.first() << ","
          << e.second() << ").";
    }
  }

  const OT::Indices getSepset(gum::NodeId x, gum::NodeId y) {
    gum::Edge e(x, y);
    if (pvalues_.exists(e)) {
      return sepset_[e];
    } else {
      throw OT::InvalidArgumentException(HERE)
          << "Error: No Sepset for edge (" << e.first() << "," << e.second()
          << ").";
    }
  }

  const std::vector<gum::Edge> &getRemoved() { return removed_; };

  std::string skeletonToDot(const gum::UndiGraph &skeleton);
  std::string PDAGtoDot(const gum::MixedGraph &pdag);

private:
  gum::EdgeProperty<OT::Indices> sepset_;
  gum::EdgeProperty<double> pvalues_;
  gum::EdgeProperty<double> ttests_;
  std::vector<gum::Edge> removed_;

  OT::UnsignedInteger maxParents_;
  bool verbose_;
  bool optimalPolicy_;
  ContinuousTTest tester_;

  bool testCondSetWithSize_(gum::UndiGraph &g, OT::UnsignedInteger n);

  std::tuple<bool, double, double, OT::Indices>
  bestSeparator_(const gum::UndiGraph &g, gum::NodeId y, gum::NodeId z,
                 const OT::Indices &neighbours, OT::UnsignedInteger n);
};
} // namespace OTAGRUM

#endif // OTAGRUM_CONTINUOUSPC_HXX