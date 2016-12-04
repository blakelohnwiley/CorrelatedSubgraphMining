#include "hashtable.h"

void Hashtable::push(string& dfs_code, Graph& g, set<string>& child)
{
	(*this)[dfs_code].push(g, child);
}

void Hashtable::push(string& dfs_code, Graph& g)
{
	(*this)[dfs_code].push(g);
}

void Hashtable::computeFrequency()
{
	for (Hashtable::hastable_iterator it = this->begin(); it != this->end(); ++it)
	{
		it->second.computeFrequency();
	}
}

void Hashtable::computeFrequency(Hashtable::hastable_iterator start)
{
	for (Hashtable::hastable_iterator it = start; it != this->end(); ++it)
	{
		it->second.computeFrequency();
	}
}

int Hashtable::freq(string dfs_code)
{
	return (*this)[dfs_code].freq;
}

void Hashtable::computeCorrelatedValue (Graph & bigGraph, Instance & ins1, Instance & ins2, int & colocated, double & confidence, int hop, int& numTestCollocated)
{
	vector<bool> I1(ins1.independent_graphs.size(), false);
	vector<bool> I2(ins2.independent_graphs.size(), false);

	for (int i = 0; i < ins1.independent_graphs.size(); i++)
	{
		for (int j = 0; j < ins2.independent_graphs.size(); j++)
		{
			++numTestCollocated;

			if (Utility::isInHHop(bigGraph, ins1.graphs[ins1.independent_graphs[i]], ins2.graphs[ins2.independent_graphs[j]], hop) == true)
			{
				I1[i] = true;
				I2[j] = true;
				break;
			}
		}
	}

	for (int i = 0; i < ins2.independent_graphs.size(); i++)
	{
		if (I2[i] == false)
		{
			for (int j = 0; j < ins1.independent_graphs.size(); j++)
			{
				++numTestCollocated;

				if (Utility::isInHHop(bigGraph, ins1.graphs[ins1.independent_graphs[j]], ins2.graphs[ins2.independent_graphs[i]], hop) == true)
				{
					I2[i] = true;
					break;
				}
			}
		}
	}

	int tmpColo1 = 0, tmpColo2 = 0;
	for (int i = 0; i < I1.size(); i++)
	{
		if (I1[i] == true)
			++tmpColo1;
	}

	for (int i = 0; i < I2.size(); i++)
	{
		if (I2[i] == true)
			++tmpColo2;
	}

	colocated = (tmpColo1 < tmpColo2) ? tmpColo1 : tmpColo2;
	
	int maxFreq = (ins1.freq > ins2.freq) ? ins1.freq : ins2.freq;

	confidence = colocated * 1.0 / maxFreq;
}