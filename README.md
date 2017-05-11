# Graph-Convexity

This repository contains an efficient implementation of an algorithm for growing [convex subgraph](https://en.wikipedia.org/wiki/Convex_subgraph) of a network.

Initial node is chosen at random from all nodes in given network.

In each step one node is chosen to be added to subgraph. It is picked randomly from nodes that have at least one neighbor in existing subgraph.
Chance to select particular node is proportional to number of neighbors that are contained subgraph.

Than additional nodes are added to subgraph until its convexity is achieved. This is performed in O(k<sub>tot</sub>*m) time, where m is number of nodes in the subgraph and k<sub>tot</sub> is total degree of nodes that are added.
Growing subgraph untill it contains all nodes of a network takes O(n*m) time, where n is number of nodes and m number of links in the network.

