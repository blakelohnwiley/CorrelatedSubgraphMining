#ifndef DUALISO_H
#define DUALISO_H

#include "graph.h"
#include "dualsimulation.h"
#include "labelcandidateselector.h"
#include "utility.h"
#include <vector>
#include <algorithm>

using namespace std;

class DualISO
{
private:
	LabelCandidateSelector candidateSelector;
	DualSimulation dualSimulation;
	vector<Graph> matches;

	void search(Graph & database, Graph & pattern, vector< vector<int> >& candidates, int depth);
	Graph toGraph(Graph & database, Graph & pattern, vector<int>& candidate);
	vector<int> getCandidate(vector< vector<int> >& candidates);
	bool isFound(Graph & database, Graph & pattern, vector< vector<int> >& candidates, int depth);

public:
	vector<Graph> match(Graph& database, Graph& pattern);
	bool isChild(Graph& database, Graph& pattern);
};

#endif