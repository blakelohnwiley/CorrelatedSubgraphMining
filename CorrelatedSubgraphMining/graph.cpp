/*
 *  
 *  Copyright 2016. All rights reserved.
 *
 */
#include "graph.h"
#include <cstring>
#include <string>
#include <set>
#include <iterator>
#include <strstream>
#include <fstream>
#include <algorithm>
using namespace std;

template <typename T,typename I>
void tokenize(const char* str, I iterator)
{
	istrstream is (str, strlen(str));
	copy (istream_iterator<T> (is), istream_iterator<T> (), iterator);
}

void Graph::buildEdge()
{
	sortGaph();

	char buf[512];
	map<string, unsigned int> tmp;
	unsigned int id = 0;

	for (int from = 0; from < (int) size(); ++from) 
	{
		for (Vertex::edge_iterator it = (*this)[from].edge.begin(); it != (*this)[from].edge.end(); ++it)
		{
			if ((*this)[from].id <= it->to)
				sprintf(buf,"%d %d %d", (*this)[from].id, it->to, it->elabel);
			else
				sprintf(buf,"%d %d %d", it->to, (*this)[from].id, it->elabel);
			
			if(tmp.find(buf) == tmp.end())
			{
				it->id = id;
				tmp[buf] = id;
				++id;
			}
			else
			{
				it->id = tmp[buf];
			}			
		}
	}
	
	edge_size_ = id;
}

int Graph::index(int id)
{
	return mapIdToIndex[id];
}

int Graph::vertexLabel(int id)
{
	return (*this)[mapIdToIndex[id]].label;
}

void Graph::insertEdge(Graph& g, const Edge& e)
{
	// check if start point and end point of e exist?
	bool isFound = false;
	for (int i = 0; i < size(); i++)
	{
		if ((*this)[i].id == e.from)
		{
			isFound = true;
			break;
		}
	}

	if (isFound == true)
	{
		isFound = false;
		for (int i = 0; i < size(); i++)
		{
			if ((*this)[i].id == e.to)
			{
				isFound = true;
				break;
			}
		}

		if (!isFound)
		{
			// Insert a new vertex that is the start point of e
			resize(size() + 1);
			(*this)[size() - 1].id = e.to;
			(*this)[size() - 1].label = g.vertexLabel(e.to);
			mapIdToIndex[e.from] = size() - 1;
		}
	}
	else
	{
		// Insert a new vertex that is the start point of e
		resize(size() + 1);
		(*this)[size() - 1].id = e.from;
		(*this)[size() - 1].label = g.vertexLabel(e.from);
		mapIdToIndex[e.from] = size() - 1;
	}

	// start point and end point of e existed, insert edge e
	(*this)[mapIdToIndex[e.from]].push(e.from, e.to, e.elabel);
	(*this)[mapIdToIndex[e.to]].push(e.to, e.from, e.elabel);

	buildEdge();
}

void Graph::sortGaph()
{
	sort(this->begin(), this->end());
	// rebuild the mapping table from Id to Index
	mapIdToIndex.clear();

	for (int i = 0; i < (int)size(); i++)
	{
		mapIdToIndex[(*this)[i].id] = i;
	}
}

bool Graph::isDuplicated(const Graph& g)
{
	// check number of vertices and edges
	if (size() != g.vertex_size())
		return false;

	if (edge_size() != g.edge_size())
		return false;

	// Check id and label of all vertices
	Graph cp1 = *this;
	cp1.sortGaph();

	Graph cp2 = g;
	cp2.sortGaph();

	for (int i = 0; i < (int)cp1.size(); i++)
	{
		if (cp1[i] != cp2[i])
			return false;
	}

	// Check id and label of all edges
	for (int i = 0; i < (int)cp1.size(); i++)
	{
		if (cp1[i].edge.size() != cp2[i].edge.size())
			return false;
		for (Vertex::edge_iterator it = cp1[i].edge.begin(); it != cp1[i].edge.end(); ++it)
		{
			bool isEdgeExist = false;
			for (Vertex::edge_iterator it1 = cp2[i].edge.begin(); it1 != cp2[i].edge.end(); ++it1)
			{
				if (*it == *it1)
				{
					isEdgeExist = true;
					break;
				}
			}

			if (!isEdgeExist)
				return false;
		}
	}

	return true;
}

int Graph::read(char* _fname)
{
	mapIdToIndex.clear();

	vector <string> result;
	char line[1024];
	int linecnt = 0;
	
	ifstream is;
	
	is.open(_fname, ios::in);
	
	if (!is)
	{
		cerr << "file open fail!!" << endl;
		return -1;
	}
	
	clear();
	
	while (true)
	{
		//cout << "Read in : ";
		unsigned int pos = is.tellg();
		if (!is.getline(line,1024))
		{
			cerr << "read all lines in file" << endl;
			break;
		}
		linecnt++;
		
		result.clear();
		tokenize<string>(line, back_inserter(result));
		
		//cout << "size:" << size() << endl;
		
		if (!result.empty())
		{
			if (result[0] == "v" && result.size() >= 3) 
			{
				int id = atoi(result[1].c_str());
				this->resize(size() + 1);
				(*this)[size() - 1].id = id;
				(*this)[size() - 1].label = atoi(result[2].c_str());
				mapIdToIndex[id] = size() - 1;
				//cout<<"v "<< atoi(result[1].c_str()) << " " << atoi(result[2].c_str()) << " " << (*this)[id].label <<" added"<< endl;
			}
			else if (result[0] == "e" && result.size() >= 4) 
			{
				int from = atoi(result[1].c_str());
				int to = atoi(result[2].c_str());
				int elabel = atoi(result[3].c_str());
				//cout << "size:" << (int)size();
				if((int)size() <= mapIdToIndex[from] || (int)size() <= mapIdToIndex[to])
				{
					cerr << "Input Format Error: define vertex lists before edges" << endl;
					exit(-1);
				}
			
				(*this)[mapIdToIndex[from]].push(from, to, elabel);
				(*this)[mapIdToIndex[to]].push(to, from, elabel);
				//cout<<"edge added"<<std::endl;
			}
		}
	}

	buildEdge();
	
	return linecnt;
}

ostream& Graph::write(ostream& os)
{
	char buf[512];
	set<string> tmp;
	
	for(int from = 0; from < (int)size(); ++from)
	{
		os << "v " << (*this)[from].id << " " << (*this)[from].label << endl;

		for(Vertex::edge_iterator it = (*this)[from].edge.begin(); it!=(*this)[from].edge.end(); ++it)
		{
			if((*this)[from].id <= it->to)
			{
				sprintf(buf,"%d %d %d", (*this)[from].id, it->to, it->elabel);
			}
			else
			{
				sprintf(buf,"%d %d %d", it->to, (*this)[from].id, it->elabel);
			}
			tmp.insert(buf);
		}
	}
	
	for(set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		os << "e " << *it << std::endl;
	}
	
	return os;
}

ofstream& Graph::write(ofstream& os)
{
	char buf[512];
	set<string> tmp;
	
	for (int from = 0; from < (int)size(); ++from)
	{
		os << "v " << (*this)[from].id << " " << (*this)[from].label << std::endl;
		
		for(Vertex::edge_iterator it = (*this)[from].edge.begin(); it != (*this)[from].edge.end(); ++it)
		{
			if((*this)[from].id <= it->to)
			{
				sprintf(buf,"%d %d %d", (*this)[from].id, it->to, it->elabel);
			}
			else
			{
				sprintf(buf,"%d %d %d", it->to, (*this)[from].id, it->elabel);
			}
			tmp.insert(buf);
		}
	}
	
	for(set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		os << "e " << *it << endl;
	}
	
	return os;
}

void Graph::check()
{
	for (int from = 0; from < (int)size(); ++from){
		cout << "check vertex " << (*this)[from].id << ",label " << (*this)[from].label << endl;
		for(Vertex::edge_iterator it = (*this)[from].edge.begin(); it != (*this)[from].edge.end(); ++it)
		{
			cout << "check edge from " << it->from << " to " << it->to << ", label " << it->elabel << endl; 
		}
	}
}
