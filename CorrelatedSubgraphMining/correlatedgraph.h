#ifndef CORRELATEDGRAPH_H
#define CORRELATEDGRAPH_H

#include "graph.h"
#include "graphcode.h"
#include "hashtable.h"
#include "treenode.h"
#include "utility.h"
#include "correlatedresult.h"
#include "TopKQueue.h"

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
	void baseLine(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, unsigned int k);
	void forwardPruning(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, unsigned int k);

	void computeCorrelatedValueBaseline(char* filenameOuput, int theta, double phi, int hop, unsigned int k);
	void ImprovedComputeCorrelatedGraph(char * filenameOuput, int theta, double phi, int hop, unsigned int k);
	void write (ofstream & of, Graph& g1, Graph& g2, int id, double colocated, double confidence);

	void topkPruning(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, unsigned int k);
	void topKComputeCorrelatedGraph(char * filenameOuput, int theta, double phi, int hop, unsigned int k);

	void baseLineInducedSubgraph(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, unsigned int k);
	void computeCorrelatedValueBaselineInducedSubgraph(char* filenameOuput, int theta, double phi, int hop, unsigned int k);

	void forwardPruningInducedSubgraph(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, unsigned int k);
	void computeCorrelatedForwardPruningInducedSubgraph(char* filenameOuput, int theta, double phi, int hop, unsigned int k);

	void topKPruningInducedSubgraph(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, unsigned int k);
	void computeCorrelatedTopKPruningInducedSubgraph(char* filenameOuput, int theta, double phi, int hop, unsigned int k);
};

#endif