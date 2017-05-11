# Graph-Convexity

This repository contains an efficient implementation of an algorithm for growing [convex subgraph](https://en.wikipedia.org/wiki/Convex_subgraph) of a network.


## vector<vector<int>> readPajek(string fn)
reads a graph from a pajek (.net) file.
fn: path to file.

returns graph (in adjecency list format)

Only works for undirected networks (directed ones will get some duplicated edges - from two-way connections)!

## vector<vector<int>> distances(const vector<vector<int>>& graph)
calculates distances between every pair of vertices in the given graph.

## vector<int> convexGrowthTriangleIneq(vector<vector<int>>& graph, vector<vector<int>>& distances, unordered_set<int>& subGraph, int newVertex)
Grow a convex subgraph by specified vertex. Adds more vertices to make new subgraph connected.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(k<sub>tot</sub>*m) time, where m is number of vertices in the subgraph and k<sub>tot</sub> is total degree of the vertices that are added.

For each added vertex checks its neighbors if they lie on any shortest part between new wertex and vertices
in existing subgraph. Adds such vertices to subgraph and repeats check for their neighbors.

Returns vector with all aded vertices (including initial one) and updates SubGraph with new vertices.

## vector<int> convexGrowth(vector<vector<int>>& graph, vector<vector<int>>& distances)
Randomly grows a convex subgraph in given graph.

Initial vertex is chosen at random from all vertices in given network.

In each step one vertex is chosen to be added to subgraph. It is picked randomly from vertices that have at least one neighbor in existing subgraph.
Chance to select particular vertex is proportional to number of neighbors that are contained subgraph.
Than convexGrowthTriangleIneq is called for new vertex.
Growing subgraph untill it contains all vertices of a network takes O(n*m) time, where n is number of vertices and m number of links in the network.

returns vector of integers. At i-th place it contains number of vertices that were added in i-th step of convex growth.