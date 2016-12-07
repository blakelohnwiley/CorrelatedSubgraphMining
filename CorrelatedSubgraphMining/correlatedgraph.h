#ifndef CORRELATEDGRAPH_H
#define CORRELATEDGRAPH_H

#include "graph.h"
#include "graphcode.h"
#include "hashtable.h"
#include "treenode.h"
#include "utility.h"
#include <fstream>
#include <iostream>
#include <deque>

using namespace std;

class CorrelatedGraph
{
private:
	Graph graph;
	Hashtable table;
	bool directed;

public:
	void initGraph(char * filename);
	void baseLine(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop);
	void pruning(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop);
	void mineCorrelatedGraphFromHashTable(char * filenameOutput, int thetaThres, double phiThres, int hop);
	void constructHashTable(double theta, double phi, double hop);
	void constructHashTableClosedGraph(char * filenameOuput, double theta, double phi, double hop);
	void write (ofstream & of, Graph& g1, Graph& g2, int id, double colocated, double confidence);
};

#endif