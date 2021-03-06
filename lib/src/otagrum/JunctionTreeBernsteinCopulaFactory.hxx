//                                               -*- C++ -*-
/**
 *  @brief JunctionTreeBernsteinCopulaFactory
 *
 *  Copyright 2010-2019 Airbus-LIP6-Phimeca
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

#ifndef OTAGRUM_JUNCTIONTREEBERNSTEINCOPULAFACTORY_HXX
#define OTAGRUM_JUNCTIONTREEBERNSTEINCOPULAFACTORY_HXX

//#include <openturns/DistributionFactoryImplementation.hxx>
#include <openturns/OT.hxx>
#include <otagrum/JunctionTreeBernsteinCopula.hxx>

namespace OTAGRUM
{

/**
 * @class JunctionTreeBernsteinCopulaFactory
 */
class OT_API JunctionTreeBernsteinCopulaFactory
  : public OT::DistributionFactoryImplementation
{
  CLASSNAME
public:
  /** Default constructor */
  JunctionTreeBernsteinCopulaFactory();

  /** Parameter constructor */
  explicit JunctionTreeBernsteinCopulaFactory(const int nbBins,
					      const double alpha = 0.1,
					      const int maximumConditioningSetSize = 5);

  /** Virtual constructor */
  virtual JunctionTreeBernsteinCopulaFactory *clone() const;

  using OT::DistributionFactoryImplementation::build;

  OT::Distribution build(const OT::Sample &sample) const;
  OT::Distribution build() const;

  JunctionTreeBernsteinCopula buildAsJunctionTreeBernsteinCopula(const OT::Sample &sample) const;
  JunctionTreeBernsteinCopula buildAsJunctionTreeBernsteinCopula() const;

 private:
  /* Bin number */
  int nbBins_;

  /* Threshold */
  double alpha_;

  /* Maximum conditional set size */
  int maximumConditioningSetSize_;
  
}; /* class JunctionTreeBernsteinCopulaFactory */

} // namespace OTAGRUM

#endif // OTAGRUM_JUNCTIONTREEBERNSTEINCOPULAFACTORY_HXX
