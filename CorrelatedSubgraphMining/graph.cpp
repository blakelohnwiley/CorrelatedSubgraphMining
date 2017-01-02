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
			labelIdx[(*this)[size() - 1].label].push_back(size() - 1);
		}
	}
	else
	{
		// Insert a new vertex that is the start point of e
		resize(size() + 1);
		(*this)[size() - 1].id = e.from;
		(*this)[size() - 1].label = g.vertexLabel(e.from);
		mapIdToIndex[e.from] = size() - 1;
		labelIdx[(*this)[size() - 1].label].push_back(size() - 1);
	}

	// start point and end point of e existed, insert edge e
	(*this)[mapIdToIndex[e.from]].push(e.from, e.to, e.elabel);
	
	if (directed == false)
	{
		(*this)[mapIdToIndex[e.to]].push(e.to, e.from, e.elabel);
	}

	buildEdge();
}

void Graph::insertEdge(Edge& e, int vertexStartLabel, int vertexEndLabel)
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
			(*this)[size() - 1].label = vertexEndLabel;
			mapIdToIndex[e.to] = size() - 1;
			labelIdx[vertexEndLabel].push_back(size() - 1);
		}
	}
	else
	{
		// Insert a new vertex that is the start point of e
		resize(size() + 1);
		(*this)[size() - 1].id = e.from;
		(*this)[size() - 1].label = vertexStartLabel;
		mapIdToIndex[e.from] = size() - 1;
		labelIdx[vertexStartLabel].push_back(size() - 1);
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
	labelIdx[v.label].push_back(size() - 1);

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
	labelIdx[v.label].push_back(size() - 1);
}

void Graph::sortGaph()
{
	sort(this->begin(), this->end());
	// rebuild the mapping table from Id to Index
	mapIdToIndex.clear();
	labelIdx.clear();

	for (int i = 0; i < (int)size(); i++)
	{
		mapIdToIndex[(*this)[i].id] = i;
		labelIdx[(*this)[i].label].push_back(i);
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

bool Graph::isDuplicated(const Graph& g) const
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
	labelIdx.clear();

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
	
	while(!is.eof())
	{
		//cout << "Read in : ";
		unsigned int pos = is.tellg();
		is.getline(line,1024, '\n');
		if (strlen(line) == 0)
			continue;

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
				labelIdx[id].push_back(size() - 1);
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

vector<int> Graph::getNeighbor(int id)
{
	int indexSource = index(id);
	vector<int> result;

	for (int i = 0; i < (*this)[indexSource].edge.size(); ++i)
	{
		result.push_back(index((*this)[indexSource].edge[i].to));
	}

	sort(result.begin(), result.end());

	return result;
}

vector<int> Graph::getVerticesByLabel(int label)
{
	return labelIdx[label];
}

bool Graph::operator==(const Graph& g) const
{
	return isDuplicated(g);
}

vector<Graph> Graph::getDownNeighborsExactGraph()
{
	if (this->size() <= 1)
		return vector<Graph>();
	vector<Graph> result;

	if (this->size() == 2)
	{
		if (directed)
		{
			Graph g(this->directed);

			if ((*this)[0].edge.size() > 0)
			{
				Vertex v1;
				v1.id = (*this)[0].id;
				v1.label = (*this)[0].label;
				g.insertVertex(v1);
			}
			else
			{
				Vertex v1;
				v1.id = (*this)[1].id;
				v1.label = (*this)[1].label;
				g.insertVertex(v1);
			}

			result.push_back(g);
		}
		else
		{
			Graph g1(this->directed);
			Graph g2(this->directed);

			Vertex v1;
			v1.id = (*this)[0].id;
			v1.label = (*this)[0].label;
			g1.insertVertex(v1);
			result.push_back(g1);

			Vertex v2;
			v2.id = (*this)[1].id;
			v2.label = (*this)[1].label;
			g2.insertVertex(v2);
			result.push_back(g2);
		}

		return result;
	}

	//if undirect graph, remove edges having id to > id from (at the same time removing 2 directions)
	// after removing edge, if vertex is isolated, then removing vertex
	for (int i = 0; i < size(); i++)
	{
		for (int j = 0; j < (*this)[i].edge.size(); j++)
		{
			if (directed)
			{
				bool isDeleted = false;
				Graph g = *this;
				g._isSorted = false;
				g[i].edge.erase(g[i].edge.begin() + j);

				int idx = g.index((*this)[i].edge[j].to);
				//if no edge to vertex[idx], delete it
				bool isFound = false;

				for (int tt = 0; tt < g.size(); tt++)
				{
					if (tt != idx)
					{
						for (int kk = 0; kk < g[tt].edge.size(); kk++)
						{
							if (g[tt].edge[kk].to == g[idx].id)
							{
								isFound = true;
								break;
							}
						}

						if (isFound == true)
							break;
					}
				}

				if (g[i].edge.size() == 0)
				{
					// remove i from g
					g.erase(g.begin() + i);
					isDeleted = true;
				}

				if (isFound == false)
				{
					g.erase(g.begin() + idx);
					isDeleted = true;
				}

				if (isDeleted == false)
				{
					// Check connected property
					if (g.isReachable(g[i].id, g[idx].id))
					{
						g.buildEdge();
						result.push_back(g);
					}
				}
				else
				{
					g.buildEdge();
					result.push_back(g);
				}
			}
			else
			{
				if ((*this)[i].edge[j].to > (*this)[i].id)
				{
					bool isDeleted = false;
					Graph g = *this;
					g._isSorted = false;

					g[i].edge.erase(g[i].edge.begin() + j);

					int idx = g.index((*this)[i].edge[j].to);
					for (int k = 0; k < g[idx].edge.size(); k++)
					{
						if (g[idx].edge[k].to == (*this)[i].id)
						{
							g[idx].edge.erase(g[idx].edge.begin() + k);
							break;
						}
					}

					if (g[i].edge.size() == 0)
					{
						// remove i from g
						g.erase(g.begin() + i);
						isDeleted = true;
					}

					if (g[idx].edge.size() == 0)
					{
						g.erase(g.begin() + idx);
						isDeleted = true;
					}

					if (isDeleted == false)
					{
						// Check connected property
						if (g.isReachable(g[i].id, g[idx].id))
						{
							g.buildEdge();
							result.push_back(g);
						}
					}
					else
					{
						g.buildEdge();
						result.push_back(g);
					}
				}
			}
		}
	}

	return result;
}

vector<Graph> Graph::getUpNeigborsExactGraph(Graph& database)
{
	vector<Graph> result;

	unordered_set<int> idCurrentGraph;
	for (int i = 0; i < this->size(); i++)
	{
		idCurrentGraph.insert((*this)[i].id);
	}

	for (int i = 0; i < this->size(); i++)
	{
		int indexSource = database.index((*this)[i].id);

		for (int k = 0; k < database[indexSource].edge.size(); ++k)
		{
			int idDes = database[indexSource].edge[k].to;
			unordered_set<int>::const_iterator itFi = idCurrentGraph.find(idDes);

			if (itFi != idCurrentGraph.end())
			{
				// just add new edge
				if (database.directed == false)
				{
					if (idDes > (*this)[i].id)
					{
						// insert new edge (2 directions)
						// check if the edge belonged to the current graph
						bool isExisted = false;
						for (int tt = 0; tt < (*this)[i].edge.size(); tt++)
						{
							if ((*this)[i].edge[tt].to == idDes && (*this)[i].edge[tt].elabel == database[indexSource].edge[k].elabel)
							{
								isExisted = true;
								break;
							}
						}

						if (!isExisted)
						{
							Graph g = *this;
							g[i].push((*this)[i].id, idDes, database[indexSource].edge[k].elabel);
							int idx = this->index(idDes);
							g[idx].push(idDes, (*this)[i].id, database[indexSource].edge[k].elabel);
							g.buildEdge();

							result.push_back(g);
						}
					}
				}
				else
				{
					bool isExisted = false;
					for (int tt = 0; tt < (*this)[i].edge.size(); tt++)
					{
						if ((*this)[i].edge[tt].to == idDes && (*this)[i].edge[tt].elabel == database[indexSource].edge[k].elabel)
						{
							isExisted = true;
							break;
						}
					}

					if (!isExisted)
					{
						// insert new edge
						Graph g = *this;
						g[i].push((*this)[i].id, idDes, database[indexSource].edge[k].elabel);
						g.buildEdge();

						result.push_back(g);
					}
				}
			}
			else
			{
				// add new edge and new vertex
				Graph g = *this;
				g._isSorted = false;

				int idx = database.index(idDes);
				Vertex v;
				v.id = idDes;
				v.label = database[idx].label;

				g.insertVertex(v);
				g[i].push((*this)[i].id, idDes, database[indexSource].edge[k].elabel);

				if (database.directed == false)
					g[g.size() - 1].push(idDes, (*this)[i].id, database[indexSource].edge[k].elabel);
				
				g.buildEdge();

				result.push_back(g);
			}
		}
	}

	return result;
}

bool Graph::isReachable(int idSource, int idDes)
{
	if (idSource == idDes)
		return true;
	unordered_set<int> visited;

	// Create a queue for BFS
    deque<int> queue;
 
    // Mark the current node as visited and enqueue it
	visited.insert(idSource);
	queue.push_back(idSource);

	while(!queue.empty())
	{
		int indx = index(queue.front());
		queue.pop_front();

		for (int i = 0; i < (*this)[indx].edge.size(); i++)
		{
			// If this adjacent node is the destination node, then 
            // return true
			if ((*this)[indx].edge[i].to == idDes)
                return true;
 
            // Else, continue to do BFS
			if (visited.find((*this)[indx].edge[i].to) == visited.end())
			{
				visited.insert((*this)[indx].edge[i].to);
				queue.push_back((*this)[indx].edge[i].to);
			}
		}
	}

	return false;
}

vector<Graph> Graph::getDownNeighborsInducedGraph()
{
	if (this->size() <= 1)
		return vector<Graph>();
	vector<Graph> result;

	if (this->size() == 2)
	{
		if (directed)
		{
			Graph g(this->directed);

			if ((*this)[0].edge.size() > 0)
			{
				Vertex v1;
				v1.id = (*this)[0].id;
				v1.label = (*this)[0].label;
				g.insertVertex(v1);
			}
			else
			{
				Vertex v1;
				v1.id = (*this)[1].id;
				v1.label = (*this)[1].label;
				g.insertVertex(v1);
			}

			result.push_back(g);
		}
		else
		{
			Graph g1(this->directed);
			Graph g2(this->directed);

			Vertex v1;
			v1.id = (*this)[0].id;
			v1.label = (*this)[0].label;
			g1.insertVertex(v1);
			result.push_back(g1);

			Vertex v2;
			v2.id = (*this)[1].id;
			v2.label = (*this)[1].label;
			g2.insertVertex(v2);
			result.push_back(g2);
		}

		return result;
	}

	for (int i = 0; i < size(); i++)
	{
		Graph g = *this;
		// remove all edge to g[i]
		if (directed == false)
		{
			for(int j = 0; j < g[i].edge.size(); j++)
			{
				int indxTo = g.index(g[i].edge[j].to);
				for (int k = 0; k < g[indxTo].edge.size(); k++)
				{
					if (g[indxTo].edge[k].to == g[i].id)
					{
						g[indxTo].edge.erase(g[indxTo].edge.begin() + k);
						break;
					}
				}
			}

			g.erase(g.begin() + i);
		}
		else
		{
			for (int j = 0; j < size(); j++)
			{
				if (j != i)
				{
					for (int k = 0; k < g[j].edge.size(); k++)
					{
						if (g[j].edge[k].to == g[i].id)
						{
							g[j].edge.erase(g[j].edge.begin() + k);
							break;
						}
					}
				}
			}

			g.erase(g.begin() + i);
		}

		g.buildEdge();

		if (g.isConnected())
		{
			result.push_back(g);
		}
	}

	return result;
}

bool Graph::isConnected()
{
	unordered_set<int> visitedId;

	if (directed)
	{
		convertDigraph2Undirect(visitedId);
	}
	else
	{
		DFSTraversal((*this)[0].id, visitedId);
	}

	if (visitedId.size() == this->size())
		return true;

	return false;
}

void Graph::DFSTraversal(int vId, unordered_set<int> & visitedId)
{
	if (visitedId.find(vId) != visitedId.end())
		return;
	visitedId.insert(vId);

	int indx = this->index(vId);

	for (int i = 0; i < (*this)[indx].edge.size(); i++)
	{
		if (visitedId.find((*this)[indx].edge[i].to) == visitedId.end())
			DFSTraversal((*this)[indx].edge[i].to, visitedId);
	}
}

void Graph::convertDigraph2Undirect(unordered_set<int> & visitedId)
{
	Graph g = *this;

	for (int i = 0; i < size(); i++)
	{
		for (Vertex::edge_iterator it = (*this)[i].edge.begin(); it != (*this)[i].edge.end(); ++it)
		{
			int indxTo = index(it->to);
			bool isFound = false;
			for (Vertex::edge_iterator it1 = g[indxTo].edge.begin(); it1 != g[indxTo].edge.end(); ++it1)
			{
				if (it1->to == (*this)[i].id)
				{
					isFound = true;
					break;
				}
			}

			if (!isFound)
			{
				g[indxTo].push(it->to, (*this)[i].id, it->elabel);
			}
		}
	}

	g.DFSTraversal(g[0].id, visitedId);
}

vector<Graph> Graph::getUpNeighborsInducedGraph(Graph& database)
{
	vector<Graph> result;

	unordered_set<int> idCurrentGraph;
	unordered_set<int> visited;

	for (int i = 0; i < this->size(); i++)
	{
		idCurrentGraph.insert((*this)[i].id);
	}

	for (int i = 0; i < this->size(); i++)
	{
		int indexSource = database.index((*this)[i].id);

		for (int k = 0; k < database[indexSource].edge.size(); ++k)
		{
			int idDes = database[indexSource].edge[k].to;
			unordered_set<int>::const_iterator itFi = idCurrentGraph.find(idDes);
			
			if (itFi == idCurrentGraph.end())
			{
				unordered_set<int>::const_iterator itVis = visited.find(idDes);
				if (itVis == visited.end())
				{
					Graph g = *this;
					g._isSorted = false;

					int idx = database.index(idDes);
					Vertex v;
					v.id = idDes;
					v.label = database[idx].label;

					g.insertVertex(v);
				
					// find all edges from v to current vertices in the graph
					for (int tt = 0; tt < database[idx].edge.size(); tt++)
					{
						if (idCurrentGraph.find(database[idx].edge[tt].to) != idCurrentGraph.end())
						{
							g[g.size() - 1].push(idDes, database[idx].edge[tt].to, database[idx].edge[tt].elabel);
							if (directed == false)
							{
								int vind = g.index(database[idx].edge[tt].to);
								g[vind].push(database[idx].edge[tt].to, idDes, database[idx].edge[tt].elabel);
							}
						}
					}
					
					if (directed)
					{
						// find all edges from the current vertices to v
						g[i].push((*this)[i].id, idDes, database[indexSource].edge[k].elabel);
						
						for (int tt = 0; tt < this->size(); tt++)
						{
							if (tt != i)
							{
								int idSour = database.index((*this)[tt].id);
								for (int uu = 0; uu < database[idSour].edge.size(); uu++)
								{
									if (database[idSour].edge[uu].to == idDes)
									{
										g[tt].push(g[tt].id, idDes, database[idSour].edge[uu].elabel);
										break;
									}
								}
							}
						}
					}

					g.buildEdge();

					result.push_back(g);
					visited.insert(idDes);
				}
			}
		}
	}

	return result;
}


Graph Graph::initAnyFrequentGraph(int threshold, int & outFreq)
{
	vector<int> label;

	for (unordered_map< int, vector<int> >::const_iterator it = labelIdx.begin(); it != labelIdx.end(); ++it)
	{
		if (it->second.size() >= threshold)
			label.push_back(it->first);
	}

	Graph g(this->directed);

	if (label.size() > 0)
	{
		int idSelect = rand() % label.size();
		// select vertex
		vector<int> vertices = labelIdx[label[idSelect]];

		double rndDouble = rand() / (1.0 * RAND_MAX);
		int indx = (int) (rndDouble * vertices.size());

		Vertex vert;
		vert.id = (*this)[vertices[indx]].id;
		vert.label = (*this)[vertices[indx]].label;

		outFreq = labelIdx[vert.label].size();

		g.insertVertex(vert);
	}

	return g;
}
