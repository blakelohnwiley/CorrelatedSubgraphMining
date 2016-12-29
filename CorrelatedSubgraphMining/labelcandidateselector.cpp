#include "labelcandidateselector.h"

vector< vector<int> > LabelCandidateSelector::getCandidates(Graph database, Graph pattern)
{
	vector< vector<int> > result(pattern.size());

	for (int id = 0; id < pattern.size(); id++)
	{
		int label = pattern[id].label;
		result[id] = database.getVerticesByLabel(label);
		sort(result[id].begin(), result[id].end());
	}

	return result;
}