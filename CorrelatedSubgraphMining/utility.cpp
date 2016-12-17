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
	for (int i = 0; i < g1.size(); i++)
	{
		// Find all neighboring node in h-hop
		vector<Vertex> neighbor;
		bigGraph.findNodeinHhop(g1[i].id, hop, neighbor);

		for (int k = 0; k < neighbor.size(); k++)
		{
			for (int j = 0; j < g2.size(); j++)
			{
				if (neighbor[k].id == g2[j].id)
					return true;
			}
		}
	}

	return false;
}

void Utility::readConfigFile (const char * fileConfigName, int& theta, double& phi, int& hop, int &k)
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
		unsigned int pos = is.tellg();
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
				k = atoi(result[1].c_str());
		}
	}
}

void Utility::intersecPointTwoGraphs (Graph& g1, Graph& g2, vector<int> & label)
{
	for (int i = 0; i < g1.size(); i++)
	{
		for (int j = 0; j < g2.size(); j++)
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
	for (set<CodeId>::const_iterator it1 = ins1.ignoreList.begin(); it1 != ins1.ignoreList.end(); ++it1)
	{
		set<CodeId>::const_iterator got = ins2.ignoreList.find(*it1);
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
	for (int i = 0; i < ins1.graphs.size(); i++)
	{
		bool isFound = false;
		int j = 0;
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

	for (int k = 0; k < mark.size(); k++)
	{
		if (mark[k] == false)
			return false;
	}

	// Check the number of share nodes
	int min = labelList[0].size();
	int minIndex = 0;
	for (int k = 1; k < labelList.size(); k++)
	{
		if (labelList[k].size() < min)
		{
			min = labelList[k].size();
			minIndex = k;
		}
	}

	for (int i = 0; i < min; i++)
	{
		// count the number of same label of index i in current list
		int countI = 0;
		for (int j = 0; j < min; j++)
			if (labelList[minIndex][i] == labelList[minIndex][j])
				countI++;
		for (int u = 0; u < labelList.size(); u++)
		{
			if (u != minIndex)
			{
				int countU = 0;
				for (int v = 0; v < labelList[u].size(); v++)
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

double Utility::pairFuntion (double k1, double k2)
{
	if (k1 > k2)
	{
		double tmp = k1;
		k1 = k2;
		k2 = tmp;
	}

	return (k1 + k2) * (k1 + k2 + 1) / 2 + k2;
}