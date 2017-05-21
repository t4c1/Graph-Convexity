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

using namespace std;

class SubGraph {
public:
	vector<char> present;
	vector<int> list;
	SubGraph(vector<vector<int>>& graph) :
		present(vector<char>(graph.size()))
	{}
	inline bool insert(int vertex) {
		bool inserted = !present[vertex];
		if (inserted) {
			present[vertex] = 1;
			list.push_back(vertex);
		}
		return inserted;
	}
};

/*
reads a graph from a pajek (.net) file.
fn: path to file.

returns graph (in adjecency list format)

Only works for undirected networks (directed ones will get some duplicated edges - from two-way connections)!
*/
vector<vector<int>> readPajek(string fn) {
	std::ifstream input(fn, std::ifstream::in);
	vector<vector<int>> res;
	char line[256];
	int n;
	input.getline(line, 256,' ');
	input >> n;
	input.getline(line, 256);
	res.resize(n,vector<int>());
	for(int i=0;i<n;i++)
	{
		input.getline(line, 256);
	}
	int m;
	input.getline(line, 256, ' ');
	input >> m;
	input.getline(line, 256);
	for (int i = 0; i<m; i++)
	{
		input.getline(line, 256);
		int a, b;
		sscanf(line, "%d %d", &a, &b);
		a--; // pajek uses 1-based indexing
		b--;
		res[a].push_back(b);
		res[b].push_back(a);
	}
	return res;
}

#define NO_VALUE -1
/*
calculates distances between every pair of vertices in the given graph.
*/
vector<vector<int>> distances(const vector<vector<int>>& graph) {
	vector<vector<int>> res;
	res.reserve(graph.size());
	for (int vertex = 0; vertex < graph.size(); vertex++) {
		vector<int> distances(graph.size(), NO_VALUE);
		distances[vertex] = 0;
		deque<int> todo;
		todo.push_back(vertex);
		while (!todo.empty()) {
			int current = todo.front(); //todo: try ref, pop after
			todo.pop_front();
			for (int neighbor : graph[current]) {
				if (distances[neighbor] == NO_VALUE) {
					distances[neighbor] = distances[current] + 1;
					todo.push_back(neighbor);
				}
			}
		}
		res.push_back(move(distances)); //todo: try skipping move
	}
	return res;//todo move?
}

/*
Grow a convex subgraph by specified vertex. Adds more vertices to make new subgraph connected.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(ktot*m) time, where m is number of vertices in the subgraph and ktot is total degree of the vertices that are added.

For each added vertex checks its neighbors if they lie on any shortest part between new wertex and vertices
in existing subgraph. Adds such vertices to subgraph and repeats check for their neighbors.

Returns vector with all aded vertices (including initial one) and updates SubGraph with new vertices.
*/
vector<int> convexGrowthTriangleIneq(vector<vector<int>>& graph, vector<vector<int>>& distances, SubGraph& subGraph, int newVertex) {
	deque<int> todo;
	todo.push_back(newVertex);
	vector<char> alreadyChecked(graph.size());//elements initialized to 0
	vector<int> insertions;
	int check_n = 1;
	alreadyChecked[newVertex] = check_n;
	insertions.push_back(newVertex);
	subGraph.insert(newVertex);
	while (!todo.empty()) {
		int current = todo.front();
		todo.pop_front();
		for (int neighbor : graph[current]) {
			if (alreadyChecked[neighbor]!=current && !subGraph.present[neighbor]) {
				alreadyChecked[neighbor] = current;
				for (int endVertex : subGraph.list) {
					if (distances[current][endVertex] >= distances[current][neighbor] + distances[neighbor][endVertex]) {
						todo.push_back(neighbor);
						insertions.push_back(neighbor);
						subGraph.insert(neighbor);
						check_n++;
						break;
					}
					else {
					}
				}
			}
		}
	}

	return insertions;
}

/*
Grow a convex subgraph by specified vertex. Adds more vertices to make new subgraph connected.

Than additional vertices are added to subgraph until its convexity is achieved. This is performed in O(ktot*m) time, where m is number of vertices in the subgraph and ktot is total degree of the vertices that are added.

For each added vertex performs a breadth first search starting at new vertex. Last discovered vertex of the subgraph determines maximum search depth - 
search stops after there are no more vertices on equal distance. During search constructs directed acyclyc graph of all shortest paths between new vertex and all discovered vertices.

Than second breadth first search is run on this graph of shortest paths starting on all vertices of the subgraph. All visited vertices are added to subgraph. Algorithm is then repeated for each added vertex.

This function is faster than convexGrowthTriangleIneq only if distances are not precomputed and final size of subgraph is much smaller than original graph. If subgraph is grown
untill it contains whole graph computing distances and using convexGrowthTriangleIneq is faster.

Returns vector with all added vertices (including initial one) and updates SubGraph with new vertices.
*/
vector<int> convexGrowthTwoSearch(vector<vector<int>>& graph, SubGraph& subGraph, int newVertex) {
	if (subGraph.list.empty()) {
		subGraph.insert(newVertex);
		return{ newVertex };
	}
	unordered_set<int> toFind(subGraph.list.begin(), subGraph.list.end());
	vector<int> dists(graph.size(),NO_VALUE);
	dists[newVertex] = 0;
	deque<int> todo;
	todo.push_back(newVertex);
	vector<vector<int>> parents(graph.size());
	vector<int> res;
	int stopDistance = INT_MAX;
	while (!todo.empty()) {
		int current = todo.front();
		todo.pop_front();
		if (dists[current]>stopDistance) {
			break;
		}
		for (int neighbor : graph[current]) {
			if (dists[neighbor]==-1) {
				dists[neighbor] = dists[current] + 1;
				todo.push_back(neighbor);
			}
			if (dists[neighbor] == dists[current] + 1) {
				toFind.erase(neighbor);
				parents[neighbor].push_back(current);
				if (toFind.empty()) {
					stopDistance = dists[current];
				}
			}
		}
	}
	todo = deque<int>(subGraph.list.begin(), subGraph.list.end());
	while (!todo.empty()) {
		int current = todo.front();
		todo.pop_front();
		if (dists[current] != NO_VALUE) { //reuse data structure
			dists[current] = NO_VALUE;
			if (subGraph.insert(current)) {//if current was not yet in subGraph
				res.push_back(current);
			}
			for (int neighbor : parents[current]) {
				todo.push_back(neighbor); //TODO perf - only if neighbor not in subGraph and not in todo
			}
		}
	}
	auto tmpRes = res;
	for (int added : tmpRes) {
		if (added != newVertex) {
			vector<int> res2=convexGrowthTwoSearch(graph, subGraph, added);
			for (int added2 : res2) {
				if (added2 != added) {
					res.push_back(added);
				}
			}
		}
	}
	return res;
}

/*
Randomly grows a convex subgraph in given graph.

Initial vertex is chosen at random from all vertices in given network.

In each step one vertex is chosen to be added to subgraph. It is picked randomly from vertices that have at least one neighbor in existing subgraph.
Chance to select particular vertex is proportional to number of neighbors that are contained subgraph.
Than convexGrowthTriangleIneq is called for new vertex.
Growing subgraph untill it contains all vertices of a network takes O(n*m) time, where n is number of vertices and m number of links in the network.

returns vector of integers. At i-th place it contains number of vertices that were added in i-th step of convex growth.
*/
vector<int> convexGrowth(vector<vector<int>>& graph, vector<vector<int>>& distances) {
	SubGraph subGraph(graph);
	vector<int> neighbors;
	std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
	//std::default_random_engine generator(334);
	for (int i = 0; i < graph.size(); i++) {
		neighbors.push_back(i);
	}
	vector<int> res;
	while (!neighbors.empty()){
		std::uniform_int_distribution<int> distribution(0, neighbors.size()-1);
		int newVertex = neighbors[distribution(generator)];
		vector<int> insertions = convexGrowthTriangleIneq(graph, distances, subGraph, newVertex);
		//vector<int> insertions = convexGrowthTwoSearch(graph, subGraph, newVertex);

		neighbors.clear();  // update neighbors of the subgraph
		for (int i : subGraph.list) {
			for (int neighbor : graph[i]) {
				if (!subGraph.present[neighbor]) {
					neighbors.push_back(neighbor);
#ifdef _DEBUG //if debugging check that result is correct
					for (int j : subGraph.list) {
						assert(distances[i][j] < distances[i][neighbor] + distances[neighbor][j]);
					}
#endif
				}
			}
		}
		res.push_back(insertions.size());
	}
	return res;
}


int main() {
	string fn = R"(C:\Users\tadej\PycharmProjects\INA\circles_lcc.net)";
	//string fn = R"(F:\Users\Tadej\Documents\fax_dn\INA\circles_lcc.net)";
	auto g = readPajek(fn);
	cout << g.size() << endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto dists = distances(g);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	cout << "dists " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;
	begin = std::chrono::steady_clock::now();
	auto res = convexGrowth(g, dists);
	end = std::chrono::steady_clock::now();
	cout << "algo " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000. / 1000. << " s" << endl;
	(cout << (std::accumulate(res.begin(), res.end(), 0)) << " " << g.size()) << endl;
	assert(std::accumulate(res.begin(), res.end(), 0) == g.size()); //sanity check - holds only for connected graphs
	return 0;
}