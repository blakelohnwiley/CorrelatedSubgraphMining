#ifndef LABELCANDIDATESELECTOR_H
#define LABELCANDIDATESELECTOR_H

#include "graph.h"
#include <vector>

using namespace std;

class LabelCandidateSelector
{
public:
	vector< vector<int> > getCandidates(Graph database, Graph pattern);
};

#endif