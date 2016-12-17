#ifndef UTILITY_H
#define UTILITY_H

#include "graph.h"
#include "graphtomindfscode.h"
#include "instance.h"
#include <iostream>
#include <vector> 

using namespace std;

class Utility
{
public:
	static bool isExistEdgeInList(vector<Edge> &, Edge &);
	static bool isExistVertexInList(vector<Vertex> &, Vertex &);
	static DFSCode computeDFSCode(Graph& g, DFSCode& code);
	static bool isInHHop (Graph & bigGraph, Graph& g1, Graph& g2, int hop);
	static void readConfigFile (const char * fileConfigName, int& theta, double& phi, int& hop, int &k);
	static void intersecPointTwoGraphs (Graph& g1, Graph& g2, vector<int> & label);

	// Check to ignore due to two graphs are parts of other graphs
	static bool isIgnore (Instance &ins1, Instance &ins2);
	static double pairFuntion (double k1, double k2);
};

#endif