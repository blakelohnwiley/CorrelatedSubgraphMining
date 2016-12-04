#ifndef UTILITY_H
#define UTILITY_H

#include "graph.h"
#include "graphtomindfscode.h"
#include <iostream>
#include <vector> 

using namespace std;

class Utility
{
public:
	static bool isExistEdgeInList(vector<Edge> &, Edge &);
	static DFSCode computeDFSCode(Graph& g, DFSCode& code);
	static bool isInHHop (Graph & bigGraph, Graph& g1, Graph& g2, int hop);
	static void readConfigFile (const char * fileConfigName, int& theta, double& phi, int& hop);
};

#endif