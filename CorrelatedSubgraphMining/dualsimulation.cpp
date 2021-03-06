#include "dualsimulation.h"

vector< vector<unsigned int> > DualSimulation::simulate(Graph & g, Graph & q, vector< vector<unsigned int> > & candidates)
{
	bool  changed = true;

    while (changed)
	{
		changed = false;
		// for each vertex u_Q in the pattern
		unsigned int qSize = q.size();

		for (unsigned int u_Q = 0; u_Q < qSize; u_Q++)
		{
			// for each neighbor of u_Q (v_Q)
			vector<unsigned int> indexNeighbor = q.getNeighbor(q[u_Q].id);
			unsigned int neighborSize = indexNeighbor.size();

			for (unsigned int v_Q = 0; v_Q < neighborSize;  v_Q++)
			{
				// keep track of candidates that have a parent in cand(u_Q)
				vector<unsigned int> v_Q_candidates;

				vector<unsigned int> indxCandLargeGraph = candidates[u_Q];
				// for each candidate of u_Q (u_G)
				for (unsigned int u_G = 0; u_G < indxCandLargeGraph.size(); u_G++)
				{
					vector<unsigned int> intersect = Utility::intersectSorted(g.getNeighbor(g[indxCandLargeGraph[u_G]].id), candidates[indexNeighbor[v_Q]]);
					// check if edge exists in the database
					if (intersect.size() == 0)
					{
						// if not, remove candidates from cand(u_Q)
						Utility::removeElement(candidates[u_Q], indxCandLargeGraph[u_G]);
						// if no candidate is left for u_Q, there is no embedding
						if (candidates[u_Q].size() == 0)
						{
							return vector < vector<unsigned int> >();
						}

						changed = true;
					}

					// update valid candidates for v_Q
					v_Q_candidates = Utility::unionSortedSet(v_Q_candidates, intersect);
				}

				// if no candidates for v_Q, there is no embedding
				if (v_Q_candidates.empty() == true)
				{
					return vector < vector<unsigned int> >();
				}

				// trigger re-eval of candidates of v_Q changed
				if (v_Q_candidates.size() < candidates[indexNeighbor[v_Q]].size())
				{
					changed = true;
				}

				candidates[indexNeighbor[v_Q]] = Utility::intersectSorted(candidates[indexNeighbor[v_Q]], v_Q_candidates);
			}
		}
	}

	return candidates;
}