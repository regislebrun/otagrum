import openturns as ot
import otagrum
import pyAgrum as gum
import sys

bn = gum.BayesNet.fastPrototype("0->1->2->3;1->4->3;4->5;6->5;7->5")
ie = gum.LazyPropagation(bn)
jtagr = ie.junctionTree()
jt = otagrum.NamedJunctionTree(jtagr, bn)

print("agrum WORLD")
for node in jtagr.nodes():
  print('clique', node, '=', jtagr.clique(node))

for edge in jtagr.edges():
  u, v = edge
  print('separator', edge, '=', jtagr.separator(u, v))
print()


print("otagurm WORLD")
for node in jt.getNodes():
  print('clique', node, '=', jt.getClique(node))

for u in jt.getNodes():
  for v in jt.neighbours(u):
    if u<v:
      print('separator', u,','v, '=', jt.getSeparator(u, v))
print()

bn = gum.fastBN("0->1->2<-0->3->4<-5->6;4->7;8->9->10<-11")
ie = gum.LazyPropagation(bn)
jt = ie.junctionTree()
njt = otagrum.NamedJunctionTree(jt, bn)

for cliq in njt.getOrderMaxFirst():
  # Only for happy Python 3.6+ users
  # print(f"{cliq} : {njt.getClique(cliq)}")
  print("clique : {}".format(type(cliq)))
  print("{} : {}".format(cliq, jt.clique(cliq)))
  print("{} : {}".format(cliq, njt.getClique(int(cliq))))
