#include "dualiso.h"

vector<Graph> DualISO::match(Graph & database, Graph & pattern)
{
	matches.clear();

	vector< vector<unsigned int> > cand_0 = candidateSelector.getCandidates(database, pattern);
    vector< vector<unsigned int> > cand = dualSimulation.simulate(database, pattern, cand_0);

	if (!cand.empty())
		search(database, pattern, cand, 0);

    return matches;
}

void DualISO::search(Graph & database, Graph & pattern, vector< vector<unsigned int> >& candidates, unsigned int depth)
{
	if (depth == pattern.size())
	{
		vector<unsigned int> cnd = getCandidate(candidates);
		Graph g = toGraph(database, pattern, cnd);
		if (g.size() > 0)
		{
			vector<Graph>::const_iterator itFind = std::find(matches.begin(), matches.end(), g);
			if (itFind == matches.end())
				matches.push_back(g);
		}
    }
	else
	{
		for (unsigned int v_G = 0; v_G < candidates[depth].size(); v_G++)
		{
			if (depth == 0 || Utility::contains(candidates, candidates[depth][v_G], depth - 1) == false)
			{
				vector< vector<unsigned int> > cand_copy = candidates;
				vector<unsigned int> tmp;
				tmp.push_back(candidates[depth][v_G]);
				cand_copy[depth] = tmp;
				vector< vector<unsigned int> > cpy = dualSimulation.simulate(database, pattern, cand_copy);

				if(!cpy.empty())
				{
					search(database, pattern, cpy, depth + 1);
				}
			}
		}
    }
}

vector<unsigned int> DualISO::getCandidate(vector< vector<unsigned int> > & candidates)
{
	unsigned int m = candidates.size();

    vector<unsigned int> result;

    for (unsigned int i = 0; i < m; i++)
	{
		result.push_back(candidates[i][0]);
    }

	return result;
}

Graph DualISO::toGraph(Graph & database, Graph & pattern, vector<unsigned int>& candidate)
{
	Graph g(database.directed);
	unsigned int patternSize = pattern.size();

	for (unsigned int i = 0; i < patternSize; i++)
	{
		g.insertVertex(database[candidate[i]]);

		unordered_map<int, int> idCandidate;

		unsigned int tmpEdgeSize = pattern[i].edge.size();

		for (unsigned int j = 0; j < tmpEdgeSize; j++)
		{
			unsigned int idx = pattern.index(pattern[i].edge[j].to);
			idCandidate[database[candidate[idx]].id] = pattern[i].edge[j].elabel;
		}

		/*vector<int> indx = pattern.getNeighbor(pattern[i].id);
		unordered_map<int, int> idCandidate;
		for (unsigned int j = 0; j < indx.size(); j++)
		{
			int label = -1;
			for (unsigned int k = 0; k < pattern[i].edge.size(); k++)
			{
				if (pattern[i].edge[k].to == pattern[indx[j]].id)
				{
					label = pattern[i].edge[k].elabel;
					break;
				}
			}
			idCandidate[database[candidate[indx[j]]].id] = label;
		}*/

		// add edge to g
		unsigned int tmpS = database[candidate[i]].edge.size();

		for (unsigned int k = 0; k < tmpS; ++k)
		{
			unordered_map<int, int>::const_iterator it = idCandidate.find(database[candidate[i]].edge[k].to);

			if (it != idCandidate.end())
			{
				if (database[candidate[i]].edge[k].elabel == it->second)
				{
					g[i].push(database[candidate[i]].edge[k].from, database[candidate[i]].edge[k].to, database[candidate[i]].edge[k].elabel);
				}
				else
				{
					return Graph(database.directed);
				}
			}
		}
	}

	g.buildEdge();

	return g;
}

bool DualISO::isChild(Graph & database, Graph & pattern)
{
	vector< vector<unsigned int> > cand_0 = candidateSelector.getCandidates(database, pattern);
    vector< vector<unsigned int> > cand = dualSimulation.simulate(database, pattern, cand_0);

	bool ok = false;
	if (!cand.empty())
		ok = isFound(database, pattern, cand, 0);

	return ok;
}

bool DualISO::isFound(Graph & database, Graph & pattern, vector< vector<unsigned int> >& candidates, unsigned int depth)
{
	if (depth == pattern.size())
	{
		vector<unsigned int> cnd = getCandidate(candidates);
		Graph g = toGraph(database, pattern, cnd);
		if (g.size() == 0)
			return false;

		return true;
    }
	else
	{
		unsigned int tmpSize = candidates[depth].size();

		for (unsigned int v_G = 0; v_G < tmpSize; v_G++)
		{
			if (depth == 0 || Utility::contains(candidates, candidates[depth][v_G], depth - 1) == false)
			{
				vector< vector<unsigned int> > cand_copy = candidates;
				vector<unsigned int> tmp;
				tmp.push_back(candidates[depth][v_G]);
				cand_copy[depth] = tmp;
				vector< vector<unsigned int> > cand_cpy = dualSimulation.simulate(database, pattern, cand_copy);

				if(!cand_cpy.empty())
				{
					bool ok = isFound(database, pattern, cand_cpy, depth + 1);
					if (ok == true)
						return true;
				}
			}
		}
    }

	return false;
}