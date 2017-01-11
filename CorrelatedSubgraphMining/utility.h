#ifndef UTILITY_H
#define UTILITY_H

#include "graph.h"
#include "graphtomindfscode.h"
#include "instance.h"
#include <iostream>
#include <vector> 
#include <stdint.h>
#include <algorithm>
#include <unordered_set>

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

	static vector<unsigned int> intersect(vector<unsigned int> f1, vector<unsigned int> f2);
	static vector<unsigned int> intersectSorted(vector<unsigned int> f1, vector<unsigned int> f2);
	static vector<unsigned int> unionSet(vector<unsigned int> f1, vector<unsigned int> f2);
	static vector<unsigned int> unionSortedSet(vector<unsigned int> f1, vector<unsigned int> f2);
	static bool contains(vector<unsigned int> & f, unsigned int e);
	static vector<unsigned int> removeElement(vector<unsigned int>& v, unsigned int e);
	static bool contains(vector< vector<unsigned int> > f, unsigned int e, unsigned int row);
};

#endif