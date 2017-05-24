#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <deque>
#include <fstream>
#include <cstdio>
#include <utility>
#include <unordered_set>
#include <random>
#include <cassert>
#include <numeric>
#include <iostream>
#include <chrono>
#include <cstring>

using namespace std;
#define NO_VALUE -1

class SubGraph {
public:
	vector<char> present;
	vector<int> list;
	SubGraph(vector<vector<int>>& network);
	bool insert(int vertex);
};

/*
reads a network from a pajek (.net) file.
fn: path to file.

returns network (in adjecency list format)

Reads network as undirected, even if it is saved as directed. Any multiedges are reduced to single edges.
*/
vector<vector<int>> readPajek(string fn);
/*
reduces network to its largest connected component
*/
vector<vector<int>> reduceToLCC(const vector<vector<int>>& network);

/*
calculates distances between every pair of vertices in the given network.
*/
vector<vector<int>> distances(const vector<vector<int>>& network);

/*
Grow a convex subgraph by specified vertex. Adds more vertices to make new subgraph connected.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(ktot*m) time, where m is number of vertices in the subgraph and ktot is total degree of the vertices that are added.

For each added vertex checks its neighbors if they lie on any shortest part between new wertex and vertices
in existing subgraph. Adds such vertices to subgraph and repeats check for their neighbors.

Returns vector with all aded vertices (including initial one) and updates SubGraph with new vertices.
*/
vector<int> convexGrowthTriangleIneq(vector<vector<int>>& network, vector<vector<int>>& distances, SubGraph& subGraph, int newVertex);

/*
Grow a convex subgraph by specified vertex. Adds more vertices to make new subgraph connected.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(ktot*m) time, where m is number of vertices in the subgraph and ktot is total degree of the vertices that are added.

For each added vertex performs a breadth first search starting at new vertex. Last discovered vertex of the subgraph determines maximum search depth -
search stops after there are no more vertices on equal distance. During search constructs directed acyclyc network of all shortest paths between new vertex and all discovered vertices.

Than second breadth first search is run on this network of shortest paths starting on all vertices of the subgraph. All visited vertices are added to subgraph. Algorithm is then repeated for each added vertex.

This function is faster than convexGrowthTriangleIneq only if distances are not precomputed and final size of subgraph is much smaller than original network. If subgraph is grown
until it contains whole network computing distances and using convexGrowthTriangleIneq is faster.

Returns vector with all added vertices (including initial one) and updates SubGraph with new vertices.
*/
vector<int> convexGrowthTwoSearch(vector<vector<int>>& network, SubGraph& subGraph, int newVertex);

/*
Randomly grows a convex subgraph in given network.

Initial vertex is chosen at random from all vertices in given network.

In each step one vertex is chosen to be added to subgraph. It is picked randomly from vertices that have at least one neighbor in existing subgraph.
Chance to select particular vertex is proportional to number of neighbors that are contained subgraph.
Than convexGrowthTriangleIneq is called for new vertex.
Growing subgraph until it contains all vertices of a network takes O(n*m) time, where n is number of vertices and m number of links in the network.

returns vector of integers. At i-th place it contains number of vertices that were added in i-th step of convex growth.
*/
vector<int> convexGrowth(vector<vector<int>>& network, vector<vector<int>>& distances);