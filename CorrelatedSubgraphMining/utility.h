#ifndef UTILITY_H
#define UTILITY_H

#include "graph.h"
#include "graphtomindfscode.h"
#include "instance.h"
#include <iostream>
#include <vector> 
#include <stdint.h>
#include <algorithm>

using namespace std;

class Utility
{
public:
	static bool isExistEdgeInList(vector<Edge> &, Edge &);
	static bool isExistVertexInList(vector<Vertex> &, Vertex &);
	static DFSCode computeDFSCode(Graph& g, DFSCode& code);
	static bool isInHHop (Graph & bigGraph, Graph& g1, Graph& g2, int hop);
	static void readConfigFile (const char * fileConfigName, int& theta, double& phi, int& hop, unsigned int &k);
	static void intersecPointTwoGraphs (Graph& g1, Graph& g2, vector<int> & label);

	// Check to ignore due to two graphs are parts of other graphs
	static bool isIgnore (Instance &ins1, Instance &ins2);
	static uint64_t pairFuntion (uint64_t k1, uint64_t k2);

	static vector<int> intersect(vector<int> f1, vector<int> f2);
	static vector<int> intersectSorted(vector<int> f1, vector<int> f2);
	static vector<int> unionSet(vector<int> f1, vector<int> f2);
	static vector<int> unionSortedSet(vector<int> f1, vector<int> f2);
	static bool contains(vector<int> & f, int e);
	static vector<int> removeElement(vector<int>& v, int e);
	static bool contains(vector< vector<int> > f, int e, int row);
};

#endif