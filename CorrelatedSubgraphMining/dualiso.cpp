#include "dualiso.h"

vector<Graph> DualISO::match(Graph database, Graph pattern)
{
	vector< vector<int> > cand_0 = candidateSelector.getCandidates(database, pattern);
    cand_0 = dualSimulation.simulate(database, pattern, cand_0);

    search(database, pattern, cand_0, 0);

    return matches;
}

void DualISO::search(Graph database, Graph pattern, vector< vector<int> > candidates, int depth)
{
	if (depth == pattern.size())
	{
		vector<int> cnd = getCandidate(candidates);
		Graph g = toGraph(database, pattern, cnd);
		vector<Graph>::const_iterator itFind = std::find(matches.begin(), matches.end(), g);
		if (itFind == matches.end())
			matches.push_back(g);
    }
	else
	{
		for (int v_G = 0; v_G < candidates[depth].size(); v_G++)
		{
			if (Utility::contains(candidates, candidates[depth][v_G], depth - 1) == false)
			{
				vector< vector<int> > cand_copy = candidates;
				vector<int> tmp;
				tmp.push_back(candidates[depth][v_G]);
				cand_copy[depth] = tmp;
				cand_copy = dualSimulation.simulate(database, pattern, cand_copy);

				if(!cand_copy.empty())
				{
					search(database, pattern, cand_copy, depth + 1);
				}
			}
		}
    }
}

vector<int> DualISO::getCandidate(vector< vector<int> > candidates)
{
	int m = candidates.size();

    vector<int> result;

    for (int i = 0; i < m; i++)
	{
		result.push_back(candidates[i][0]);
    }

	return result;
}

Graph DualISO::toGraph(Graph database, Graph pattern, vector<int> candidate)
{
	Graph g(database.directed);

	for (int i = 0; i < pattern.size(); i++)
	{
		g.insertVertex(database[candidate[i]]);

		vector<int> indx = pattern.getNeighbor(pattern[i].id);
		vector<int> idCandidate;
		for (int j = 0; j < indx.size(); j++)
		{
			idCandidate.push_back(database[candidate[indx[j]]].id);
		}

		// add edge to g
		for (int k = 0; k < database[candidate[i]].edge.size(); ++k)
		{
			vector<int>::const_iterator it = std::find(idCandidate.begin(), idCandidate.end(), database[candidate[i]].edge[k].to);

			if (it != idCandidate.end())
			{
				g[i].push(database[candidate[i]].edge[k].from, database[candidate[i]].edge[k].to, database[candidate[i]].edge[k].elabel);
			}
		}
	}

	g.buildEdge();

	return g;
}