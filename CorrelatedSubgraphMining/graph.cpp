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
	unordered_map<string, unsigned int> tmp;
	unsigned int id = 0;

	unsigned int size_ = size();

	for (unsigned int from = 0; from < size_; ++from) 
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

unsigned int Graph::index(int id)
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
	unsigned int size_ = size();
	for (unsigned int i = 0; i < size_; i++)
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
		for (unsigned int i = 0; i < size_; i++)
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
			Vertex tmpV;
			tmpV.id = e.to;
			tmpV.label = g.vertexLabel(e.to);
			this->push_back(tmpV);
			size_ = size();
			mapIdToIndex[e.to] = size_ - 1;
			labelIdx[(*this)[size_ - 1].label].push_back(size_ - 1);
		}
	}
	else
	{
		// Insert a new vertex that is the start point of e
		Vertex tmpV;
		tmpV.id = e.from;
		tmpV.label = g.vertexLabel(e.from);
		this->push_back(tmpV);
		size_ = size();
		mapIdToIndex[e.from] = size_ - 1;
		labelIdx[(*this)[size_ - 1].label].push_back(size_ - 1);
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
	unsigned int size_ = size();
	for (unsigned int i = 0; i < size_; i++)
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
		for (unsigned int i = 0; i < size_; i++)
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
			Vertex tmpV;
			tmpV.id = e.to;
			tmpV.label = vertexEndLabel;
			this->push_back(tmpV);

			size_ = size();
			mapIdToIndex[e.to] = size_ - 1;
			labelIdx[vertexEndLabel].push_back(size_ - 1);
		}
	}
	else
	{
		// Insert a new vertex that is the start point of e
		Vertex tmpV;
		tmpV.id = e.from;
		tmpV.label = vertexStartLabel;
		this->push_back(tmpV);

		size_ = size();
		mapIdToIndex[e.from] = size_ - 1;
		labelIdx[vertexStartLabel].push_back(size_ - 1);
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
	Vertex tmpV;
	tmpV.id = v.id;
	tmpV.label = v.label;
	this->push_back(tmpV);

	unsigned int size_ = size();

	mapIdToIndex[v.id] = size_ - 1;
	labelIdx[v.label].push_back(size_ - 1);

	// get all edges to and from this node from g
	unsigned int indexBigGraph = g.mapIdToIndex[v.id];

	if (!g.directed)
	{
		for (vector<Edge>::iterator it = g[indexBigGraph].edge.begin(); it != g[indexBigGraph].edge.end(); ++it)
		{
			unordered_map<int, unsigned int>::iterator itMap = mapIdToIndex.find(it->to);
			if (itMap != mapIdToIndex.end())
			{
				(*this)[itMap->second].push(it->to, it->from, it->elabel);
				(*this)[size_ - 1].push(it->from, it->to, it->elabel);
			}
		}
	}
	else
	{
		// to v
		for (unsigned int i = 0; i < size_ - 1; i++)
		{
			unsigned int indexG = g.mapIdToIndex[(*this)[i].id];

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
			unordered_map<int, unsigned int>::iterator itMap = mapIdToIndex.find(it->to);
			if (itMap != mapIdToIndex.end())
			{
				(*this)[size_ - 1].push(it->from, it->to, it->elabel);
			}
		}
	}

	buildEdge();
}

void Graph::insertVertex(Vertex& v)
{
	Vertex tmpV;
	tmpV.id = v.id;
	tmpV.label = v.label;

	this->push_back(tmpV);

	unsigned int size_ = size();

	mapIdToIndex[v.id] = size_ - 1;
	labelIdx[v.label].push_back(size_ - 1);
}

void Graph::sortGaph()
{
	sort(this->begin(), this->end());
	// rebuild the mapping table from Id to Index
	mapIdToIndex.clear();
	labelIdx.clear();

	unsigned int size_ = size();

	for (unsigned int i = 0; i < size_ ; i++)
	{
		mapIdToIndex[(*this)[i].id] = i;
		labelIdx[(*this)[i].label].push_back(i);
	}

	this->_isSorted = true;
}

bool Graph::isExist(Edge& e)
{
	unsigned int size_ = size();

	for (unsigned int i = 0; i < size_; i++)
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
	unsigned int size_ = size();

	for (unsigned int i = 0; i < size_; i++)
	{
		if ((*this)[i] == v)
			return true;
	}

	return false;
}

bool Graph::overlap (Graph& g)
{
	unsigned int size_ = size();
	unsigned int g_size = g.vertex_size();

	for (unsigned int i = 0; i < size_; i++)
	{
		for (unsigned int j = 0; j < g_size; j++)
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

	unsigned int cp1_size = cp1.size();

	for (unsigned int i = 0; i < cp1_size; i++)
	{
		if (cp1[i] != cp2[i])
			return false;
	}

	// Check id and label of all edges
	for (unsigned int i = 0; i < cp1_size; i++)
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

				Vertex tmpV;
				tmpV.id = id;
				tmpV.label = atoi(result[2].c_str());
				this->push_back(tmpV);

				unsigned int size_ = size();
				mapIdToIndex[id] = size_ - 1;
				labelIdx[tmpV.label].push_back(size_ - 1);
			}
			else if (result[0] == "e" && result.size() >= 4) 
			{
				int from = atoi(result[1].c_str());
				int to = atoi(result[2].c_str());
				int elabel = atoi(result[3].c_str());
				
				unsigned int size_ = size();

				if(size_ <= mapIdToIndex[from] || size_ <= mapIdToIndex[to])
				{
					cerr << "Input Format Error: define vertex lists before edges" << endl;
					exit(-1);
				}
			
				(*this)[mapIdToIndex[from]].push(from, to, elabel);
				
				if (directed == false)
				{
					(*this)[mapIdToIndex[to]].push(to, from, elabel);
				}
			}
		}
	}

	//buildEdge();
	
	return linecnt;
}

ostream& Graph::write(ostream& os)
{
	char buf[512];
	unordered_set<string> tmp;
	
	unsigned int size_ = size();

	for(unsigned int from = 0; from < size_; ++from)
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
	
	for(unordered_set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		os << "e " << *it << std::endl;
	}
	
	return os;
}

ofstream& Graph::write(ofstream& os)
{
	char buf[512];
	unordered_set<string> tmp;
	
	unsigned int size_ = size();

	for (unsigned int from = 0; from < size_; ++from)
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
	
	for(unordered_set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		os << "e " << *it << endl;
	}
	
	return os;
}

void Graph::check()
{
	unsigned int size_ = size();

	for (unsigned int from = 0; from < size_; ++from){
		cout << "check vertex " << (*this)[from].id << ",label " << (*this)[from].label << endl;

		for(Vertex::edge_iterator it = (*this)[from].edge.begin(); it != (*this)[from].edge.end(); ++it)
		{
			cout << "check edge from " << it->from << " to " << it->to << ", label " << it->elabel << endl; 
		}
	}
}

void Graph::findNodeinHhop(const int & vertexId, const int & hop, vector<Vertex>& results)
{
	unsigned int ind = mapIdToIndex[vertexId];

	deque<Vertex> Queue;
	(*this)[ind].depth = 0;
	Queue.push_back((*this)[ind]);

	while (!Queue.empty())
	{
		Vertex v = Queue.front();
		unsigned int sizeV = v.edge.size();

		for (unsigned int i = 0; i < sizeV; i++)
		{
			unsigned int idex = mapIdToIndex[v.edge[i].to];
			Vertex u = (*this)[idex];
			bool notIn = true;

			//check u is in results?
			for (unsigned int j = 0; j < results.size(); j++)
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
				unsigned int sizeQ = Queue.size();
				for (unsigned int j = 0; j < sizeQ; j++)
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

vector<unsigned int> Graph::getNeighbor(int id)
{
	unsigned int indexSource = index(id);
	vector<unsigned int> result;
	unsigned int size_ = (*this)[indexSource].edge.size();

	for (unsigned int i = 0; i < size_; ++i)
	{
		result.push_back(index((*this)[indexSource].edge[i].to));
	}

	sort(result.begin(), result.end());

	return result;
}

vector<unsigned int> Graph::getVerticesByLabel(int label)
{
	return labelIdx[label];
}

bool Graph::operator==(const Graph& g) const
{
	return isDuplicated(g);
}

vector<Graph> Graph::getDownNeighborsExactGraph()
{
	unsigned int currentSize = this->size();

	if (currentSize <= 1)
		return vector<Graph>();

	vector<Graph> result;

	if (currentSize == 2)
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
	for (unsigned int i = 0; i < currentSize; i++)
	{
		unsigned int edgeSize = (*this)[i].edge.size();
		for (unsigned int j = 0; j < edgeSize; j++)
		{
			if (directed)
			{
				bool isDeleted = false;
				Graph g = *this;
				g._isSorted = false;
				g[i].edge.erase(g[i].edge.begin() + j);

				unsigned int idx = g.index((*this)[i].edge[j].to);
				
				//if no edge to vertex[idx], delete it
				bool isFound = false;

				unsigned int g_size = g.size();

				for (unsigned int tt = 0; tt < g_size; tt++)
				{
					if (tt != idx)
					{
						unsigned int edge_g_size = g[tt].edge.size();

						for (unsigned int kk = 0; kk < edge_g_size; kk++)
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

				if (g[i].edge.empty() == true)
				{
					// remove i from g
					g[i] = g.back();
					g.pop_back();
					//g.erase(g.begin() + i);
					isDeleted = true;
				}

				if (isDeleted == false && isFound == false)
				{
					g[idx] = g.back();
					g.pop_back();
					//g.erase(g.begin() + idx);
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
					//cout << "Starting Delete edge j" << endl;
					g[i].edge.erase(g[i].edge.begin() + j);
					//cout << "Deleted edge j" << endl;
					unsigned int idx = g.index((*this)[i].edge[j].to);
					//cout << "Index" << endl;
					unsigned int gidx_edge_size = g[idx].edge.size();
					for (unsigned int k = 0; k < gidx_edge_size; k++)
					{
						if (g[idx].edge[k].to == (*this)[i].id)
						{
							g[idx].edge.erase(g[idx].edge.begin() + k);
							break;
						}
					}
					//cout << "End Index" << endl;
					if (g[i].edge.empty() == true)
					{
						// remove i from g
						g[i] = g.back();
						g.pop_back();
						//g.erase(g.begin() + i);
						isDeleted = true;
						//cout << "Deleted vertex i" << endl;
					}

					if (isDeleted == false)
					{
						if (g[idx].edge.empty())
						{
							g[idx] = g.back();
							g.pop_back();
							//g.erase(g.begin() + idx);
							isDeleted = true;
							//cout << "Deleted vertex idx" << endl;
						}
					}

					if (isDeleted == false)
					{
						// Check connected property
						//cout << "Check Connected" << endl;
						if (g.isReachable(g[i].id, g[idx].id))
						{
							g.buildEdge();
							result.push_back(g);
						}
						//cout << "End Check Connected" << endl;
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
	unsigned int currentSize = this->size();

	unordered_set<int> idCurrentGraph;
	for (unsigned int i = 0; i < currentSize; i++)
	{
		idCurrentGraph.insert((*this)[i].id);
	}

	for (unsigned int i = 0; i < currentSize; i++)
	{
		unsigned int indexSource = database.index((*this)[i].id);
		unsigned int edgeSizetmp = database[indexSource].edge.size();

		for (unsigned int k = 0; k < edgeSizetmp; ++k)
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
						unsigned int edgePatternG = (*this)[i].edge.size();
						for (unsigned int tt = 0; tt < edgePatternG; tt++)
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
							unsigned int idx = this->index(idDes);
							g[idx].push(idDes, (*this)[i].id, database[indexSource].edge[k].elabel);
							g.buildEdge();

							result.push_back(g);
						}
					}
				}
				else
				{
					bool isExisted = false;
					unsigned int edgePatterSi = (*this)[i].edge.size();
					for (unsigned int tt = 0; tt < edgePatterSi; tt++)
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

				unsigned int idx = database.index(idDes);

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
		unsigned int indx = index(queue.front());
		queue.pop_front();

		unsigned int edgeSize = (*this)[indx].edge.size();
		for (unsigned int i = 0; i < edgeSize; i++)
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
	unsigned int currentSize = this->size();

	if (currentSize <= 1)
		return vector<Graph>();

	vector<Graph> result;

	if (currentSize == 2)
	{
		if (directed)
		{
			Graph g(this->directed);

			if ((*this)[0].edge.empty() == false)
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

	for (unsigned int i = 0; i < currentSize; i++)
	{
		Graph g = *this;
		// remove all edge to g[i]
		if (directed == false)
		{
			unsigned int edge_i_size = g[i].edge.size();
			for(unsigned int j = 0; j < edge_i_size; j++)
			{
				unsigned int indxTo = g.index(g[i].edge[j].to);
				unsigned int edgeTmpSi = g[indxTo].edge.size();

				for (unsigned int k = 0; k < edgeTmpSi; k++)
				{
					if (g[indxTo].edge[k].to == g[i].id)
					{
						g[indxTo].edge[k] = g[indxTo].edge.back();
						g[indxTo].edge.pop_back();
						break;
					}
				}
			}

			g[i] = g.back();
			g.pop_back();
			//g.erase(g.begin() + i);
		}
		else
		{
			for (unsigned int j = 0; j < currentSize; j++)
			{
				if (j != i)
				{
					unsigned int tmpSize = g[j].edge.size();
					for (unsigned int k = 0; k < tmpSize; k++)
					{
						if (g[j].edge[k].to == g[i].id)
						{
							g[j].edge[k] = g[j].edge.back();
							g[j].edge.pop_back();
							//g[j].edge.erase(g[j].edge.begin() + k);
							break;
						}
					}
				}
			}

			g[i] = g.back();
			g.pop_back();
			//g.erase(g.begin() + i);
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

	unsigned int indx = this->index(vId);
	unsigned int sizeTmp = (*this)[indx].edge.size();

	for (unsigned int i = 0; i < sizeTmp; i++)
	{
		if (visitedId.find((*this)[indx].edge[i].to) == visitedId.end())
			DFSTraversal((*this)[indx].edge[i].to, visitedId);
	}
}

void Graph::convertDigraph2Undirect(unordered_set<int> & visitedId)
{
	Graph g = *this;
	unsigned int size_ = size();

	for (unsigned int i = 0; i < size_; i++)
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
	unsigned int size_ = this->size();

	for (unsigned int i = 0; i < size_; i++)
	{
		idCurrentGraph.insert((*this)[i].id);
	}

	for (unsigned int i = 0; i < size_; i++)
	{
		int indexSource = database.index((*this)[i].id);
		unsigned int edgeTmpSize = database[indexSource].edge.size();

		for (unsigned int k = 0; k < edgeTmpSize; ++k)
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

					unsigned int idx = database.index(idDes);

					Vertex v;
					v.id = idDes;
					v.label = database[idx].label;

					g.insertVertex(v);
				
					// find all edges from v to current vertices in the graph
					unsigned int tmpS = database[idx].edge.size();
					for (unsigned int tt = 0; tt < tmpS; tt++)
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
						
						for (unsigned int tt = 0; tt < size_; tt++)
						{
							if (tt != i)
							{
								unsigned int idSour = database.index((*this)[tt].id);
								unsigned int tmpS1 = database[idSour].edge.size();

								for (unsigned int uu = 0; uu < tmpS1; uu++)
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

	for (unordered_map< int, vector<unsigned int> >::const_iterator it = labelIdx.begin(); it != labelIdx.end(); ++it)
	{
		if ((int)it->second.size() >= threshold)
			label.push_back(it->first);
	}

	Graph g(this->directed);

	if (label.size() > 0)
	{
		unsigned int idSelect = rand() % label.size();
		// select vertex
		vector<unsigned int> vertices = labelIdx[label[idSelect]];

		double rndDouble = rand() / (1.0 * RAND_MAX);
		unsigned int indx = rndDouble * vertices.size();

		Vertex vert;
		vert.id = (*this)[vertices[indx]].id;
		vert.label = (*this)[vertices[indx]].label;

		outFreq = labelIdx[vert.label].size();

		g.insertVertex(vert);
	}

	return g;
}
