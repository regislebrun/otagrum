//                                               -*- C++ -*-
/**
 *  @brief JunctionTree
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

#ifndef OTAGR_JUNCTIONTREE_HXX
#define OTAGR_JUNCTIONTREE_HXX

#include <string>
#include <vector>

#include <agrum/core/hashTable.h>
#include <agrum/graphs/cliqueGraph.h>

#include <openturns/Description.hxx>
#include <openturns/Indices.hxx>

namespace OTAGR {
class JunctionTree {
private:
  gum::JunctionTree jt_;
  OT::Description map_;

  OT::Indices fromNodeSet_(const gum::NodeSet &clique);

  void checkConsistency_();

public:
  JunctionTree(const gum::JunctionTree &jt,
               const std::vector<std::string> &names);

  ~JunctionTree();

  OT::UnsignedInteger getSize();

  OT::Description getDescription();

  OT::Indices getClique(gum::NodeId nod);
  OT::Indices getSeparator(gum::Edge edge);

  const gum::NodeSet &getNeighbours(gum::NodeId id);
  gum::EdgeSet getEdges();
  gum::NodeSet getNodes();

  OT::Collection<OT::Indices> getCliquesCollection();
  OT::Collection<OT::Indices> getSeparatorCollection();

  JunctionTree getMarginal(OT::Indices indices);

  std::string __str__();
};
};
#endif // OTAGR_JUNCTIONTREE_H
