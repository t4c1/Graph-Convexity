# Network-Convexity: Growth of convex subgraph in networks(graphs)

This repository contains an efficient implementation of an algorithm for growing [convex subgraph](https://en.wikipedia.org/wiki/Convex_subgraph) of a network.

An induced subgraph is convex, if all shortest (geodesic) paths between vertices of the subgraph consist only of vertices of the subgraph.
In other words no shortest path lie outside subgraph.

We can grow convex subgraph by random neighboring vertex. We add specified vertex to subgraph, than check shortest paths between vertices of the new subgraph.
Any vertices not in subgraph are added, than shortest paths are rechacked. Process is repeated until no new vertices are added to subgraph.

Further read: [Convexity in complex networks](https://arxiv.org/pdf/1608.03402.pdf).

### SubGraph
A class representing subgraph. The data about subgaph is duplicated in vector<char> present and vector<int> list to allow more efficient processing.

##### SubGraph(vector<vector<int>>& network)
Constructor takes garph as argument to construct an empty subgraph.

##### vector<char> present
An indicator vector. For each vertex in network vector contains one value. It is true if vertex is present in subgraph, otherwise false.

##### vector<int> list
A vector, that contains all vertices of the subgraph.

### vector<vector<int>> readPajek(string fn)
reads a network from a pajek (.net) file.
fn: path to file.

returns network (in adjecency list format)

Reads network as undirected, even if it is saved as directed. Any multiedges are reduced to single edges.

### vector<vector<int>> reduceToLCC(const vector<vector<int>>& network)
Reduces network to its largest connected component. Input network remains unmodified.

Returns network consisiting of largest connected component of input.

### vector<vector<int>> distances(const vector<vector<int>>& network)
Calculates distances between every pair of vertices in the given network. 

The network must be undirected (a network returnd from readPajek is OK).

Returns distance matrix `D`. Distance between vertices i and j in in `D[i][j]` (and in `D[j][i]`).

### vector<int> convexGrowthTriangleIneq(vector<vector<int>>& network, vector<vector<int>>& distances, SubGraph& subGraph, int newVertex)
Grow a convex subgraph by specified vertex.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(k<sub>tot</sub>*m) time, where m is number of vertices in the subgraph and k<sub>tot</sub> is total degree of the vertices that are added.

For each added vertex checks its neighbors if they lie on any shortest part between new wertex and vertices
in existing subgraph. Adds such vertices to subgraph and repeats check for their neighbors.

Returns vector with all aded vertices (including initial one) and updates SubGraph with new vertices.

### vector<int> convexGrowthTwoSearch(vector<vector<int>>& network, SubGraph& subGraph, int newVertex)
Grow a convex subgraph by specified vertex.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(k<sub>tot</sub>*m) time, where m is number of vertices in the subgraph and k<sub>tot</sub> is total degree of the vertices that are added.

For each added vertex performs a breadth first search starting at new vertex. Last discovered vertex of the subgraph determines maximum search depth - 
search stops after there are no more vertices on equal distance. During search constructs directed acyclyc network of all shortest paths between new vertex and all discovered vertices.

Than second breadth first search is run on this network of shortest paths starting on all vertices of the subgraph. All visited vertices are added to subgraph. Algorithm is then repeated for each added vertex.

This function is faster than `convexGrowthTriangleIneq` only if distances are not precomputed and final size of subgraph is much smaller than original network. If subgraph is grown
until it contains whole network computing distances and using `convexGrowthTriangleIneq` is faster.

Returns vector with all added vertices (including initial one) and updates SubGraph with new vertices.

### vector<int> convexGrowth(vector<vector<int>>& network, vector<vector<int>>& distances)
Randomly grows a convex subgraph in given network. The network must be connected. If it is not run reduceToLCC on it first to get its largest connected component.

Initial vertex is chosen at random from all vertices in given network.

In each step one vertex is chosen to be added to subgraph. It is picked randomly from vertices that have at least one neighbor in existing subgraph.
Chance to select particular vertex is proportional to number of neighbors that are contained in the subgraph.
Than `convexGrowthTriangleIneq` is called for the new vertex.
Growing subgraph until it contains all vertices of a network takes O(n*m) time, where n is number of vertices and m number of links in the network.

Returns vector of integers. At i-th place it contains number of vertices that were added in i-th step of convex growth.