# Network-Convexity: Growth of convex subgraphs in networks (graphs)

This repository contains an efficient implementation of an algorithm for growing [convex subgraph](https://en.wikipedia.org/wiki/Convex_subgraph) of a network.

An induced subgraph is convex, if all shortest (geodesic) paths between vertices of the subgraph consist only of vertices of the subgraph.
In other words no shortest path lies outside the subgraph.

We can grow convex subgraph by random neighboring vertex. We add specified vertex to subgraph, than check shortest paths between vertices of the new subgraph.
Any vertices not in subgraph are added, than shortest paths are rechecked. Process is repeated until no new vertices are added to subgraph.

This implementation can calculate the number of vertices that were added to subraph in each step and also *c-convexity* and *maximum size of convex subsets* measures.

Further reading: [Convexity in complex networks](https://arxiv.org/pdf/1608.03402.pdf).



# Building
This projects has no dependancies, but requires c++11 support. To enable paralelism by OpenMP appropriate compiler switch should be set. 
Example compiling comand line application using g++:

`g++ -std=c++11 -fopenmp -O3 algo.cpp cli.cpp -o network-convexity`



# Command line usage
`network-convexity input [repeats [max_steps [cGlobal [cLocal [rawOut]]]]]`

`input`: path to pajek (.net) file, containing the network. This is the only mandatory parameter.

`repeats`: number of repeats of the algorithm. Since the algorithm is stohastic, larger values give more accurate results, but take more time to compute. Default is 100.

`max_steps`: Maximum number of steps the alghoritm will calculate. Default is -1, which means no limit.

`cGlobal`: Parameter for calculation of *c-convexity*. Default is 1.

`cLocal`: Parameter for calculation of *maximum size of convex subsets*. Default is 1.

rawOut: path to file in which to save raw output in csv format. Each line represents one execution of the algorithm.
i-th value in line is the number of nodes that were added to subgraph in i-th step (first two values in each line always equal to 1).
Lines can be of different length.



# Library documentation
### `SubGraph`
A class representing subgraph. The data about subgaph is duplicated in `vector<char> present` and
 `vector<int> list` to allow for more efficient processing.

##### `SubGraph(vector<vector<int>>& network)`
Constructor takes graph as an argument to construct an empty subgraph.

##### `vector<char> present`
An indicator vector. For each vertex in network vector contains one value. It is true if vertex is present in subgraph, otherwise false.

##### `vector<int> list`
A vector that contains all vertices of the subgraph.


### `vector<vector<int>> readPajek(string fn)`
reads a network from a pajek (.net) file.

`fn`: path to file

Returns the network (in adjecency list format).

Reads network as undirected, even if it is saved as directed. Any multi-edges are reduced to single edges.

### `vector<vector<int>> reduceToLCC(const vector<vector<int>>& network)`
Reduces network to its largest connected component. Input network remains unmodified.

Returns network consisiting of largest connected component of input.

### `vector<vector<int>> distances(const vector<vector<int>>& network)`
Calculates distances between every pair of vertices in the given network. 

The network must be undirected (a network returned from readPajek is OK).

Returns distance matrix `D`. Distance between vertices i and j is in `D[i][j]` (and in `D[j][i]`).

### `vector<int> convexGrowthTriangleIneq(vector<vector<int>>& network, const vector<vector<int>>& distances, SubGraph& subGraph, int newVertex)`
Grow a convex subgraph by specified vertex.

Then additional vertices are added to subgraph until its convexity is achieved. This is performed in O(k<sub>tot</sub>*n<sub>s</sub>) time, where n<sub>s</sub> 
is number of vertices in the subgraph and k<sub>tot</sub> is total degree of the vertices that are added.

For each added vertex checks its neighbors if they lie on any shortest part between new vertex and vertices
in existing subgraph. Adds such vertices to subgraph and repeats check for their neighbors.

Returns vector with all added vertices (including the initial one) and updates SubGraph with new vertices.

### `vector<int> convexGrowthTwoSearch(const vector<vector<int>>& network, SubGraph& subGraph, int newVertex)`
Grow a convex subgraph by specified vertex.

Then additional vertices are added to subgraph until its convexity is achieved. This is performed in O(k<sub>tot</sub>*n<sub>s</sub>) time, where n<sub>s</sub> is 
number of vertices in the subgraph and k<sub>tot</sub> is total degree of the vertices that are added.

For each added vertex performs a breadth first search starting at new vertex. Last discovered vertex of the subgraph determines maximum search depth - 
search stops after there are no more vertices on equal distance. During search constructs directed acyclic graph of all shortest paths between new vertex and all discovered vertices.

Than second breadth first search is run on this network of shortest paths starting on all vertices of the subgraph. All visited vertices are added to subgraph. Algorithm is then repeated for each added vertex.

This function is faster than `convexGrowthTriangleIneq` only if distances are not precomputed and final size of subgraph is much smaller than original network. If subgraph is grown
until it contains whole network, computing distances and using `convexGrowthTriangleIneq` is faster.

Returns vector with all added vertices (including initial one) and updates SubGraph with new vertices.

### `vector<int> convexGrowth(const vector<vector<int>>& network, const vector<vector<int>>& distances, int max_steps)`
Randomly grows a convex subgraph in given network. The network must be connected. If it is not run `reduceToLCC` on it first to get its largest connected component.

Initial vertex is chosen at random from all vertices in given network.

In each step one vertex is chosen to be added to subgraph. It is picked randomly from vertices that have at least one neighbor in existing subgraph.
Chance to select particular vertex is proportional to number of its neighbors that lie in the subgraph.
Than `convexGrowthTriangleIneq` is called for the new vertex.
At most `max_steps` steps are calculated. `max_steps` defaults to -1, which means no limit.

Growing subgraph until it contains all vertices of a network takes O(n<sub>s</sub>*m<sub>s</sub>) time, where n<sub>s</sub> is number of vertices and m<sub>s</sub> number of links in final subgraph.

Returns vector of integers. At i-th place it contains number of vertices that were added in i-th step of convex growth.

### `double cConvexity_Xc(const vector<int>& growths, int n, double c = 1.0)`
Calculates *c-convexity* of a network as defined in [Convexity in complex networks](https://arxiv.org/pdf/1608.03402.pdf).

`growths`: vector of integers. `growths[i]` represents number of vertices, by which subgraph grew in step i.

`n`: number of vertices in the graph.

`c`: parameter of *c-convexity* measure.

This function should be called on result of `convexGrowth`. For accurate results `convexGrowth` should be run multiple times.
`cConvexity_Xc` should be computed each time and return values should be averaged.


### `double maxConvexSubsetSize_Lc(const vector<int>& growths, double c = 1.0)`
Calculates maximum size of convex subsets of a network as defined in [Convexity in complex networks](https://arxiv.org/pdf/1608.03402.pdf).

`growths`: vector of integers. `growths[i]` represents number of vertices, by which subgraph grew in step i.

`c`: parameter of *maximum size of convex subsets* measure.

This function should be called on result of `convexGrowth`. For accurate results `convexGrowth` should be run multiple times.
`maxConvexSubsetSize_Lc` should be computed each time and return values should be averaged.