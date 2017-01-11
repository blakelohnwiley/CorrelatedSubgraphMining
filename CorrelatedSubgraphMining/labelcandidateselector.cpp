#include "labelcandidateselector.h"

vector< vector<unsigned int> > LabelCandidateSelector::getCandidates(Graph database, Graph pattern)
{
	unsigned int patternSize = pattern.size();

	vector< vector<unsigned int> > result(patternSize);

	for (unsigned int id = 0; id < patternSize; id++)
	{
		int label = pattern[id].label;
		result[id] = database.getVerticesByLabel(label);
		sort(result[id].begin(), result[id].end());
	}

	return result;
}