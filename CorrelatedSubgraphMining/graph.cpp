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
	std::copy(istream_iterator<T> (is), istream_iterator<T> (), iterator);
}

Graph::Graph(Graph* g)
{
	*this = *g;
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
			if (directed || (*this)[from].id <= it->to)
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

//void Graph::pushHHopCode(CodeId& id)
//{
//	sameHHop.insert(id);
//}

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
			// Insert a new vertex that is the end point of e
			resize(size() + 1);
			(*this)[size() - 1].id = e.to;
			(*this)[size() - 1].label = g.vertexLabel(e.to);
			mapIdToIndex[e.to] = size() - 1;
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
	
	if (directed == false)
	{
		(*this)[mapIdToIndex[e.to]].push(e.to, e.from, e.elabel);
	}

	buildEdge();
}

void Graph::insertEdge(Edge& e, int vertexStartLable, int vertexEndLable)
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
			// Insert a new vertex that is the end point of e
			resize(size() + 1);
			(*this)[size() - 1].id = e.to;
			(*this)[size() - 1].label = vertexEndLable;
			mapIdToIndex[e.to] = size() - 1;
		}
	}
	else
	{
		// Insert a new vertex that is the start point of e
		resize(size() + 1);
		(*this)[size() - 1].id = e.from;
		(*this)[size() - 1].label = vertexStartLable;
		mapIdToIndex[e.from] = size() - 1;
	}

	// start point and end point of e existed, insert edge e
	(*this)[mapIdToIndex[e.from]].push(e.from, e.to, e.elabel);

	if (directed == false)
	{
		(*this)[mapIdToIndex[e.to]].push(e.to, e.from, e.elabel);
	}

	buildEdge();
}

void Graph::extendByVertex(Graph& g, Vertex& v)
{
	resize(size() + 1);
	(*this)[size() - 1].id = v.id;
	(*this)[size() - 1].label = v.label;
	mapIdToIndex[v.id] = size() - 1;

	// get all edges to and from this node from g
	int indexBigGraph = g.mapIdToIndex[v.id];
	if (!g.directed)
	{
		for (vector<Edge>::iterator it = g[indexBigGraph].edge.begin(); it != g[indexBigGraph].edge.end(); ++it)
		{
			map<int, int>::iterator itMap = mapIdToIndex.find(it->to);
			if (itMap != mapIdToIndex.end())
			{
				(*this)[itMap->second].push(it->to, it->from, it->elabel);
				(*this)[size() - 1].push(it->from, it->to, it->elabel);
			}
		}
	}
	else
	{
		// to v
		for (int i = 0; i < size() - 1; i++)
		{
			int indexG = g.mapIdToIndex[(*this)[i].id];
			for (vector<Edge>::iterator it = g[indexG].edge.begin(); it != g[indexG].edge.end(); ++it)
			{
				if (it->to == v.id)
				{
					(*this)[i].push(it->from, it->to, it->elabel);
					break;
				}
			}
		}
		// from v
		for (vector<Edge>::iterator it = g[indexBigGraph].edge.begin(); it != g[indexBigGraph].edge.end(); ++it)
		{
			map<int, int>::iterator itMap = mapIdToIndex.find(it->to);
			if (itMap != mapIdToIndex.end())
			{
				(*this)[size() - 1].push(it->from, it->to, it->elabel);
			}
		}
	}

	buildEdge();
}

void Graph::insertVertex(Vertex& v)
{
	this->resize(size() + 1);
	(*this)[size() - 1].id = v.id;
	(*this)[size() - 1].label = v.label;
	mapIdToIndex[v.id] = size() - 1;
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

	this->_isSorted = true;
}

bool Graph::isExist(Edge& e)
{
	for (int i = 0; i < size(); i++)
	{
		for (Vertex::edge_iterator it = (*this)[i].edge.begin(); it != (*this)[i].edge.end(); ++it)
		{
			if (*it == e)
			{
				return true;
			}
		}
	}

	return false;
}

bool Graph::isExisedVertice(Vertex& v)
{
	for (int i = 0; i < size(); i++)
	{
		if ((*this)[i] == v)
			return true;
	}

	return false;
}

bool Graph::overlap (Graph& g)
{
	for (unsigned int i = 0; i < size(); i++)
	{
		for (unsigned int j = 0; j < g.vertex_size(); j++)
		{
			if ((*this)[i] == g[j])
			{
				return true;
			}
		}
	}

	return false;
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
	if (cp1.isSorted() == false)
	{
		cp1.sortGaph();
	}

	Graph cp2 = g;
	if (cp2.isSorted() == false)
	{
		cp2.sortGaph();
	}

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
				
				if (directed == false)
				{
					(*this)[mapIdToIndex[to]].push(to, from, elabel);
				}
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
			if(directed || (*this)[from].id <= it->to)
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
			if(directed || (*this)[from].id <= it->to)
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

void Graph::findNodeinHhop(const int & vertexId, const int & hop, vector<Vertex>& results)
{
	int ind = mapIdToIndex[vertexId];

	deque<Vertex> Queue;
	(*this)[ind].depth = 0;
	Queue.push_back((*this)[ind]);

	while (!Queue.empty())
	{
		Vertex v = Queue.front();

		for (int i = 0; i < v.edge.size(); i++)
		{
			int idex = mapIdToIndex[v.edge[i].to];
			Vertex u = (*this)[idex];
			bool notIn = true;

			//check u is in results?
			for (int j = 0; j < results.size(); j++)
			{
				if (results[j].id == u.id)
				{
					notIn = false;
					break;
				}
			}

			if (notIn)
			{
				// check u is in Queue?
				for (int j = 0; j < Queue.size(); j++)
				{
					if (Queue[j].id == u.id)
					{
						notIn = false;
						break;
					}
				}

				if (notIn)
				{
					u.depth = v.depth + 1;
					if (u.depth >= hop)
					{
						results.push_back(u);
					}
					else
					{
						Queue.push_back(u);
					}
				}
			}
		}

		results.push_back(v);
		Queue.pop_front();
	}
}
