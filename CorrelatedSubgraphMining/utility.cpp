#include "utility.h"
#include <iterator>
#include <strstream>
#include <cstring>
#include <string>
#include <fstream>

template <typename t,typename i>
void token(const char* str, i iterator)
{
	istrstream is (str, strlen(str));
	std::copy(istream_iterator<t> (is), istream_iterator<t> (), iterator);
}

bool Utility::isExistEdgeInList(vector<Edge> & edgeList, Edge & e)
{
	for (vector<Edge>::iterator it = edgeList.begin(); it < edgeList.end(); ++it)
	{
		if ((*it) == e)
		{
			return true;
		}
	}

	return false;
}

bool Utility::isExistVertexInList(vector<Vertex> & vertexList, Vertex & v)
{
	for (vector<Vertex>::iterator it = vertexList.begin(); it < vertexList.end(); ++it)
	{
		if ((*it) == v)
		{
			return true;
		}
	}

	return false;
}

DFSCode Utility::computeDFSCode(Graph& g, DFSCode& code)
{
	GraphToMinDFSCode convertGraphToDFSCode;
	convertGraphToDFSCode.findMinimumDFSCode(&g, code);

	return code;
}

bool Utility::isInHHop (Graph & bigGraph, Graph& g1, Graph& g2, int hop)
{
	unsigned int g1Size = g1.size();
	unsigned int g2Size = g2.size();

	for (unsigned int i = 0; i < g1Size; i++)
	{
		// Find all neighboring node in h-hop
		vector<Vertex> neighbor;
		bigGraph.findNodeinHhop(g1[i].id, hop, neighbor);
		unsigned int neiborSize = neighbor.size();
		for (unsigned int k = 0; k < neiborSize; k++)
		{
			for (unsigned int j = 0; j < g2Size; j++)
			{
				if (neighbor[k].id == g2[j].id)
					return true;
			}
		}
	}

	return false;
}

void Utility::readConfigFile (const char * fileConfigName, int& theta, double& phi, int& hop, unsigned int &k)
{
	ifstream is;
	char line[1024];
	vector<string> result;

	is.open(fileConfigName, ios::in);
	
	if (!is)
	{
		cerr << "file open fail!!" << endl;
		return;
	}

	int linecount = 0;

	while (true)
	{
		//cout << "Read in : ";
		if (!is.getline(line,1024))
		{
			cerr << "read all lines in file" << endl;
			break;
		}
		
		++linecount;

		result.clear();
		token<string>(line, back_inserter(result));
		
		if (!result.empty())
		{
			if (linecount == 1)
				theta = atoi(result[1].c_str());
			else if (linecount == 2)
				phi = atof(result[1].c_str());
			else if (linecount == 3)
				hop = atoi(result[1].c_str());
			else if (linecount == 4)
				k = (unsigned int) atoi(result[1].c_str());
		}
	}
}

void Utility::intersecPointTwoGraphs (Graph& g1, Graph& g2, vector<int> & label)
{
	for (unsigned int i = 0; i < g1.size(); i++)
	{
		for (unsigned int j = 0; j < g2.size(); j++)
		{
			if (g1[i] == g2[j])
			{
				label.push_back(g1[i].label);
				break;
			}
		}
	}
}

bool Utility::isIgnore (Instance &ins1, Instance &ins2)
{
	// check ignore list first
	for (unordered_set<CodeId>::const_iterator it1 = ins1.ignoreList.begin(); it1 != ins1.ignoreList.end(); ++it1)
	{
		unordered_set<CodeId>::const_iterator got = ins2.ignoreList.find(*it1);
		if (got != ins2.ignoreList.end())
		{
			return true;
		}
	}

	// Check the condition to be a part of another larger graph
	vector< vector<int> > labelList;
	//vector<bool> mark(ins2.independent_graphs.size(), false);
	vector<bool> mark(ins2.graphs.size(), false);

	//for (int i = 0; i < ins1.independent_graphs.size(); i++)
	for (unsigned int i = 0; i < ins1.graphs.size(); i++)
	{
		bool isFound = false;
		unsigned int j = 0;
		//for (; j < ins2.independent_graphs.size(); j++)
		for (; j < ins2.graphs.size(); j++)
		{
			if (ins1.graphs[0].size() > 2 && ins2.graphs[0].size() > 2)
			{
				if (ins1.graphs[0][1].label == 2 && ins1.graphs[0][2].label == 3 && ins2.graphs[0][1].label == 2 && ins2.graphs[0][2].label == 3)
				{
					int a = 1;
				}
			}
			vector<int> label;
			//intersecPointTwoGraphs(ins1.graphs[ins1.independent_graphs[i]], ins2.graphs[ins2.independent_graphs[j]], label);
			intersecPointTwoGraphs(ins1.graphs[i], ins2.graphs[j], label);
			
			if (label.size() >= 2)
			{
				mark[j] = true;
				isFound = true;
				//sort(label.begin(), label.end());
				labelList.push_back(label);
				//break;
			}
		}

		//if (j >= ins2.independent_graphs.size())
		if (!isFound)
		{
			return false;
		}
	}

	for (unsigned int k = 0; k < mark.size(); k++)
	{
		if (mark[k] == false)
			return false;
	}

	// Check the number of share nodes
	unsigned int min = labelList[0].size();
	int minIndex = 0;
	for (unsigned int k = 1; k < labelList.size(); k++)
	{
		if (labelList[k].size() < min)
		{
			min = labelList[k].size();
			minIndex = k;
		}
	}

	for (unsigned int i = 0; i < min; i++)
	{
		// count the number of same label of index i in current list
		int countI = 0;
		for (unsigned int j = 0; j < min; j++)
			if (labelList[minIndex][i] == labelList[minIndex][j])
				countI++;
		for (unsigned int u = 0; u < labelList.size(); u++)
		{
			if (u != minIndex)
			{
				int countU = 0;
				for (unsigned int v = 0; v < labelList[u].size(); v++)
				{
					if (labelList[minIndex][i] == labelList[u][v])
						countU++;
				}

				if (countI != countU)
					return false;
			}
		}
	}
	CodeId code;
	code.id1 = ins1.graphs[0].idGraph;
	code.id2 = ins2.graphs[0].idGraph;
	ins1.ignoreList.insert(code);
	ins2.ignoreList.insert(code);
	return true;
}

uint64_t Utility::pairFuntion (uint64_t k1, uint64_t k2)
{
	if (k1 > k2)
	{
		uint64_t tmp = k1;
		k1 = k2;
		k2 = tmp;
	}

	return (k1 + k2) * (k1 + k2 + 1) / 2 + k2;
}

vector<unsigned int> Utility::intersect(vector<unsigned int> f1, vector<unsigned int> f2)
{
	vector<unsigned int> result;

	unsigned int f1Size = f1.size();
	unsigned int f2Size = f2.size();

	for (unsigned int i = 0; i < f1Size; i++)
	{
		for (unsigned int j = 0; j < f2Size; j++)
		{
			if (f1[i] == f2[j])
			{
				result.push_back(f1[i]);
			}
		}
	}

	return result;
}

vector<unsigned int> Utility::intersectSorted(vector<unsigned int> f1, vector<unsigned int> f2)
{
	vector<unsigned int> result;

	unsigned int i = 0;
    unsigned int j = 0;
	unsigned int m = f1.size();
	unsigned int n = f2.size();

	while (i < m && j < n)
	{
		if (f1[i] < f2[j])
			++i;
		else if (f2[j] < f1[i])
			++j;
		else
		{
			result.push_back(f1[i]);
			++i;
			++j;
		}
	}

	return result;
}

vector<unsigned int> Utility::unionSet(vector<unsigned int> f1, vector<unsigned int> f2)
{
	vector<unsigned int> result;

	unsigned int f1Size = f1.size();
	unsigned int f2Size = f2.size();

	for (unsigned int i = 0; i < f1Size; i++)
	{
		if (!contains(result, f1[i]))
		{
			result.push_back(f1[i]);
		}
    }

	for (unsigned int j = 0; j < f2Size; j++)
	{
		if (!contains(result, f2[j]))
		{
			result.push_back(f2[j]);
		}
    }

	return result;
}

vector<unsigned int> Utility::unionSortedSet(vector<unsigned int> f1, vector<unsigned int> f2)
{
	vector<unsigned int> result;

	unsigned int i = 0;
    unsigned int j = 0;
	unsigned int m = f1.size();
	unsigned int n = f2.size();
    unsigned int previous = 0;

    while (i < m && j < n)
	{
		if (f1[i] < f2[j])
		{
			if (previous < f1[i])
			{
				previous = f1[i];
				result.push_back(previous);
			}
			else if (result.empty() == true && previous == f1[i])
			{
				result.push_back(previous);
			}

			i++;
		}
		else if (f1[i] > f2[j])
		{
			if (previous < f2[j])
			{
				previous = f2[j];
				result.push_back(previous);
			}
			else if (result.empty() == true && previous == f2[i])
			{
				result.push_back(previous);
			}

			j++;
		}
		else
		{
			if (previous < f1[i])
			{
				previous = f1[i];
				result.push_back(previous);
			}
			else if (result.empty() == true && previous == f1[i])
			{
				result.push_back(previous);
			}

			i++;
			j++;
		}
    }

    while (i < m)
	{
		result.push_back(f1[i]);
		++i;
    }

    while (j < n)
	{
		result.push_back(f2[j]);
		++j;
    }

	return result;
}

bool Utility::contains(vector<unsigned int> & f, unsigned int e)
{
	bool res = false;
	unsigned int fSize = f.size();

	for (unsigned int i = 0; i < fSize; i++)
	{
		if (f[i] == e)
		{
			res = true;
			break;
		}
    }

    return res;
}

vector<unsigned int> Utility::removeElement(vector<unsigned int>& v, unsigned int e)
{
	vector<unsigned int>::iterator position = std::find(v.begin(), v.end(), e);
	if (position != v.end())
		v.erase(position);

	return v;
}

bool Utility::contains(vector< vector<unsigned int> > f, unsigned int e, unsigned int row)
{
	bool res = false;

    unsigned int bound = f.size();
	bound = bound - 1;
	if (bound > row)
		bound = row;

    for (unsigned int i = 0; i <= bound; i++)
	{
		if (contains(f[i], e))
		{
			res = true;
			break;
		}
    }

    return res;
}