#include "utility.h"
#include <iterator>
#include <strstream>

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

void Utility::readConfigFile (const char * fileConfigName, int& theta, double& phi, int& hop)
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
		}
	}
}