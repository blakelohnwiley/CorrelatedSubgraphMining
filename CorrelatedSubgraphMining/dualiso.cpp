#include "dualiso.h"

vector<Graph> DualISO::match(Graph & database, Graph & pattern)
{
	matches.clear();

	vector< vector<int> > cand_0 = candidateSelector.getCandidates(database, pattern);
    vector< vector<int> > cand = dualSimulation.simulate(database, pattern, cand_0);

	if (!cand.empty())
		search(database, pattern, cand, 0);

    return matches;
}

void DualISO::search(Graph & database, Graph & pattern, vector< vector<int> >& candidates, int depth)
{
	if (depth == pattern.size())
	{
		vector<int> cnd = getCandidate(candidates);
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
			if (Utility::contains(candidates, candidates[depth][v_G], depth - 1) == false)
			{
				vector< vector<int> > cand_copy = candidates;
				vector<int> tmp;
				tmp.push_back(candidates[depth][v_G]);
				cand_copy[depth] = tmp;
				vector< vector<int> > cpy = dualSimulation.simulate(database, pattern, cand_copy);

				if(!cpy.empty())
				{
					search(database, pattern, cpy, depth + 1);
				}
			}
		}
    }
}

vector<int> DualISO::getCandidate(vector< vector<int> > & candidates)
{
	int m = candidates.size();

    vector<int> result;

    for (int i = 0; i < m; i++)
	{
		result.push_back(candidates[i][0]);
    }

	return result;
}

Graph DualISO::toGraph(Graph & database, Graph & pattern, vector<int>& candidate)
{
	Graph g(database.directed);

	for (unsigned int i = 0; i < pattern.size(); i++)
	{
		g.insertVertex(database[candidate[i]]);

		unordered_map<int, int> idCandidate;

		for (unsigned int j = 0; j < pattern[i].edge.size(); j++)
		{
			int idx = pattern.index(pattern[i].edge[j].to);
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
		for (unsigned int k = 0; k < database[candidate[i]].edge.size(); ++k)
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
	vector< vector<int> > cand_0 = candidateSelector.getCandidates(database, pattern);
    vector< vector<int> > cand = dualSimulation.simulate(database, pattern, cand_0);

	bool ok = false;
	if (!cand.empty())
		ok = isFound(database, pattern, cand, 0);

	return ok;
}

bool DualISO::isFound(Graph & database, Graph & pattern, vector< vector<int> >& candidates, int depth)
{
	if (depth == pattern.size())
	{
		vector<int> cnd = getCandidate(candidates);
		Graph g = toGraph(database, pattern, cnd);
		if (g.size() == 0)
			return false;

		return true;
    }
	else
	{
		for (unsigned int v_G = 0; v_G < candidates[depth].size(); v_G++)
		{
			if (Utility::contains(candidates, candidates[depth][v_G], depth - 1) == false)
			{
				vector< vector<int> > cand_copy = candidates;
				vector<int> tmp;
				tmp.push_back(candidates[depth][v_G]);
				cand_copy[depth] = tmp;
				vector< vector<int> > cand_cpy = dualSimulation.simulate(database, pattern, cand_copy);

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