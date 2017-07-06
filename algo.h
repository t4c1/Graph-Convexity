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
	SubGraph(const vector<vector<int>>& network);
	bool insert(int vertex);
};

vector<vector<int>> readPajek(string fn, vector<string>* names=nullptr);

void writePajek(string fn, vector<vector<int>> graph, vector<string> names);

vector<vector<int>> reduceToLCC(const vector<vector<int>>& network);

vector<vector<int>> distances(const vector<vector<int>>& network);

vector<int> convexGrowthTriangleIneq(const vector<vector<int>>& network, const vector<vector<int>>& distances, SubGraph& subGraph, int newVertex);

vector<int> convexGrowthTwoSearch(const vector<vector<int>>& network, SubGraph& subGraph, int newVertex);

vector<int> convexGrowth(const vector<vector<int>>& network, const vector<vector<int>>& distances, int max_steps = -1);

double cConvexity_Xc(const vector<int>& growths, int n, double c = 1.0);

double maxConvexSubsetSize_Lc(const vector<int>& growths, double c = 1.0);

