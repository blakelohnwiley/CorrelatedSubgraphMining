#ifndef SAMPLING_H
#define SAMPLING_H

#include "dualiso.h"
#include "graph.h"
#include "overlapgraph.h"
#include "graphtomindfscode.h"
#include "hashtable.h"
#include "TopKQueue.h"
#include "instance.h"
#include "utility.h"
#include <unordered_set>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <stdint.h>

using namespace std;

struct Sample
{
	Graph p;
	DFSCode dfs_code;
	vector<Graph> isomophisms;
	bool status;
	int freq;
};

class Sampling
{
private:
	Graph database;
	bool directed;
	DualISO dualISO;
	Hashtable table;

public:
	static uint64_t id;
	static Hashtable saveGraph;
	static unordered_set<DFSCode> noneFrequentGraph;

	static void clearStaticVariable()
	{
		saveGraph.clear();
		id = 0;
		noneFrequentGraph.clear();
	}

	int computeFrequency(vector<Graph>& listGraph);

	Sampling(bool directed_);
	Sample uniformSamplingExactGraph(int threshold, int miniter, int maxIter);
	Sample uniformSamplingInducedGraph(int threshold, int miniter, int maxIter);
	void computeCorrelatedValueUniformSamplingExactGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration);
	void computeCorrelatedValueUniformSamplingInducedGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration);

	Sample supportBiasedSamplingExactGraph(int threshold, int miniter, int maxIter);
	Sample supportBiasedSamplingInducedGraph(int threshold, int miniter, int maxIter);
	void computeCorrelatedValueSupportBiasedSamplingExactGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration);
	void computeCorrelatedValueSupportBiasedSamplingInducedGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration);

	void initGraph(char * fileInput);
};

#endif