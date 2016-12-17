#include "correlatedgraph.h"
#include <time.h>
#include <queue>

void CorrelatedGraph::initGraph(char * filename)
{
	graph.read(filename);
}

void CorrelatedGraph::baseLine(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, int k)
{
	cout << "Running baseline" << endl;
	this->directed = directed;
	graph.directed = directed;
	initGraph(filenameInput);
	clock_t start, end;

	start = clock();
	constructHashTable(filenameOuput, theta, phi, hop, k);
	//mineCorrelatedGraphFromHashTable(filenameOuput, theta, phi, hop, k);
	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;
	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::forwardPruning(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, int k)
{
	cout << "Running Forward pruning" << endl;
	this->directed = directed;
	graph.directed = directed;
	initGraph(filenameInput);
	clock_t start, end;

	start = clock();
	ImprovedComputeCorrelatedGraph(filenameOuput, theta, phi, hop, k);
	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;

	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::topkPruning(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, int k)
{
	cout << "Running Top-k Pruning" << endl;
	this->directed = directed;
	graph.directed = directed;
	initGraph(filenameInput);
	clock_t start, end;

	start = clock();
	topKComputeCorrelatedGraph(filenameOuput, theta, phi, hop, k);
	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;

	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::topKComputeCorrelatedGraph(char * filenameOuput, int theta, double phi, int hop, int k)
{
	TopKQueue topKqueue(k);

	double id = 0;
	deque<TreeNode> mainQ;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id);
		node.graph.directed = this->directed;
		node.graph.insertVertex(graph[i]);
		node.graph.idGraph = id;
		++id;
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
		Hashtable::iterator findIT = table.find(it->code);
		if (findIT != table.end())
		{
			it->isSavedToTable = true;
		}
		else
		{
			it->isSavedToTable = false;
		}
		table.push(it->code, it->graph);
	}

	//compute frequence of instances in the hashtable
	for (Hashtable::iterator it = table.begin(); it != table.end(); ++it)
	{
		it->second.computeFrequency();
	}
	int colocated;
	double confidence;
	int countPair = 0;
	int numTestHHop = 0;

	// remove all graphs being less frequent from queue
	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		Hashtable::iterator itTable = table.find(it->code);
		if (itTable != table.end())
		{
			if (itTable->second.freq < theta || (topKqueue.isFull() &&  itTable->second.freq < topKqueue.minCorrelatedValue()))
			{
				it->isStop = true;
				table.erase(itTable);
			}
			else
			{
				if (it->isSavedToTable == false)
				{
					// Compute Correlated value
					for (Hashtable::iterator iht = table.begin(); iht != itTable; ++iht)
					{
						colocated = 0;
						confidence = 0;
						table.computeCorrelatedValueClose(graph, itTable->second, iht->second, colocated, confidence, hop, numTestHHop);

						if (colocated >= theta && confidence >= phi)
						{
							++countPair;
							CorrelatedResult res;
							res.g1 = itTable->second.graphs[0];
							res.g2 = iht->second.graphs[0];
							res.colocatedvalue = colocated;
							res.confidencevalue = confidence;
							topKqueue.insert(res);
							//write(of, itTable->second.graphs[0], iht->second.graphs[0], countPair, colocated, confidence);
						}
					}
				}
			}
		}
		else
		{
			it->isStop = true;
		}
	}
	bool stop = false;

	deque<TreeNode> tmpQ;

	while (!stop)
	{
		while (!mainQ.empty())
		{
			TreeNode current = mainQ.front();

			if (!current.isStop)
			{
				Hashtable::iterator iht = table.find(current.code);
				current.ignoreList = iht->second.ignoreList;

				// find all neighbouring edges of the graph in current node
				vector<Edge> edges;
				for (vector<Vertex>::iterator itG = current.graph.begin(); itG != current.graph.end(); ++itG)
				{
					int idNode = graph.index(itG->id);
					for (Vertex::edge_iterator itE = graph[idNode].edge.begin(); itE != graph[idNode].edge.end(); itE++)
					{
						if (!current.graph.isExist(*itE) && !Utility::isExistEdgeInList(edges, *itE))
						{
							// add new edge into the candidate set
							edges.push_back(*itE);
						}
					}
				}

				// Create new graph from the candidate set of edges
				for (int ii = 0; ii < edges.size(); ii++)
				{
					Graph gtmp = current.graph;
					gtmp.directed = this->directed;
					gtmp.insertEdge(edges[ii], graph[graph.index(edges[ii].from)].label, graph[graph.index(edges[ii].to)].label);

					// Check new graph exists in tmpQ or not?
					bool isExist = false;
					for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
					{
						if (itTmpQ->isDuplicated(gtmp))
						{
							isExist = true;
							itTmpQ->childIDs.insert(current.code);
							itTmpQ->childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						}
					}

					if (!isExist)
					{
						TreeNode newnode(id);
						newnode.graph = gtmp;
						newnode.graph.idGraph = id;
						newnode.ignoreList = current.ignoreList;
						++id;
						newnode.childIDs.insert(current.code);
						newnode.childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						tmpQ.push_back(newnode);
					}
				}
			}
			
			mainQ.pop_front();
		}

		if (tmpQ.size() > 0)
		{	
			//add new graphs to hashtable
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				itTmpQ->computeDFSCode();
				Hashtable::iterator findIT = table.find(itTmpQ->code);
				if (findIT != table.end())
				{
					itTmpQ->isSavedToTable = true;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
				}
				else
				{
					itTmpQ->isSavedToTable = false;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs, itTmpQ->ignoreList);
				}
			}

			// compute frequency
			for (Hashtable::iterator ht = table.begin(); ht != table.end(); ++ht)
			{
				ht->second.computeFrequency();
			}

			// check which candidates are able to extend in tmpQ
			int count = 0;
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				Hashtable::iterator itFind = table.find(itTmpQ->code);
				if (itFind != table.end())
				{
					if (itFind->second.freq < theta || (topKqueue.isFull() &&  itFind->second.freq < topKqueue.minCorrelatedValue()))
					{
						itTmpQ->isStop = true;
						table.erase(itFind);
						++count;
					}
				}
				else
				{
					itTmpQ->isStop = true;
					++count;
				}
			}

			if (count == tmpQ.size())
			{
				stop = true;
			}
			else
			{
				for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
				{
					if (itTmpQ->isStop == false && itTmpQ->isSavedToTable == false)
					{
						Hashtable::iterator itFind = table.find(itTmpQ->code);
						// Compute Correlated value
						for (Hashtable::iterator iht = table.begin(); iht != itFind; ++iht)
						{
							bool isChild = false;
							set<DFSCode>::const_iterator got = itFind->second.childIDs.find(iht->first);
							if (got != itFind->second.childIDs.end())
							{
								isChild = true;
							}
								
							if (isChild == false)
							{
								got = iht->second.childIDs.find(itTmpQ->code);
								if (got != iht->second.childIDs.end())
								{
									isChild = true;
								}
							}
			
							if (isChild == false && Utility::isIgnore(itFind->second, iht->second) == false)
							{
								colocated = 0;
								confidence = 0;
								table.computeCorrelatedValueClose(graph, itFind->second, iht->second, colocated, confidence, hop, numTestHHop);

								if (colocated >= theta && confidence >= phi)
								{
									++countPair;
									CorrelatedResult res;
									res.g1 = itFind->second.graphs[0];
									res.g2 = iht->second.graphs[0];
									res.colocatedvalue = colocated;
									res.confidencevalue = confidence;
									topKqueue.insert(res);
								}
							}
						}
					}
				}

				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}

	ofstream of;
	of.open(filenameOuput);
	topKqueue.print(of);

	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}

void CorrelatedGraph::constructHashTable(char* filenameOuput, int theta, double phi, int hop, int k)
{
	TopKQueue saveResult(k);
	
	//ofstream of;
	//of.open("result1.txt");
	cout << "Building hashtable....." << endl;
	double id = 0;
	deque<TreeNode> mainQ;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id);
		node.graph.directed = this->directed;
		node.graph.insertVertex(graph[i]);
		node.graph.idGraph = id;
		++id;
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
		Hashtable::iterator findIT = table.find(it->code);
		if (findIT != table.end())
		{
			it->isSavedToTable = true;
		}
		else
		{
			it->isSavedToTable = false;
		}
		table.push(it->code, it->graph);
	}

	//compute frequence of instances in the hashtable
	for (Hashtable::iterator it = table.begin(); it != table.end(); ++it)
	{
		it->second.computeFrequency();
	}
	int colocated;
	double confidence;
	int countPair = 0;
	int numTestHHop = 0;

	// remove all graphs being less frequent from queue
	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		Hashtable::iterator itTable = table.find(it->code);
		if (itTable != table.end())
		{
			if (itTable->second.freq < theta)
			{
				it->isStop = true;
				table.erase(itTable);
			}
			else
			{
				if (it->isSavedToTable == false)
				{
					// Compute Correlated value
					for (Hashtable::iterator iht = table.begin(); iht != itTable; ++iht)
					{
						colocated = 0;
						confidence = 0;
						table.computeCorrelatedValueClose(graph, itTable->second, iht->second, colocated, confidence, hop, numTestHHop);

						if (colocated >= theta && confidence >= phi)
						{
							++countPair;
							CorrelatedResult res;
							res.g1 = itTable->second.graphs[0];
							res.g2 = iht->second.graphs[0];
							res.colocatedvalue = colocated;
							res.confidencevalue = confidence;
							saveResult.insert(res);
							//write(of, itTable->second.graphs[0], iht->second.graphs[0], countPair, colocated, confidence);
						}
					}
				}
			}
		}
		else
		{
			it->isStop = true;
		}
	}
	bool stop = false;

	deque<TreeNode> tmpQ;

	while (!stop)
	{
		while (!mainQ.empty())
		{
			TreeNode current = mainQ.front();

			/*of << endl;
			of << "Graph: " << endl;
			current.graph.write(of);
			of << "Code: " << current.code << endl;
			of << "Child: " << endl;
			for (int kk = 0; kk < current.childIDs.size(); kk++)
			{
				of << current.childIDs[kk] << endl;
			}*/


			if (!current.isStop)
			{
				Hashtable::iterator iht = table.find(current.code);
				current.ignoreList = iht->second.ignoreList;

				// find all neighbouring edges of the graph in current node
				vector<Edge> edges;
				for (vector<Vertex>::iterator itG = current.graph.begin(); itG != current.graph.end(); ++itG)
				{
					int idNode = graph.index(itG->id);
					for (Vertex::edge_iterator itE = graph[idNode].edge.begin(); itE != graph[idNode].edge.end(); itE++)
					{
						if (!current.graph.isExist(*itE) && !Utility::isExistEdgeInList(edges, *itE))
						{
							// add new edge into the candidate set
							edges.push_back(*itE);
						}
					}
				}

				// Create new graph from the candidate set of edges
				for (int ii = 0; ii < edges.size(); ii++)
				{
					Graph gtmp = current.graph;
					gtmp.directed = this->directed;
					gtmp.insertEdge(edges[ii], graph[graph.index(edges[ii].from)].label, graph[graph.index(edges[ii].to)].label);

					// Check new graph exists in tmpQ or not?
					bool isExist = false;
					for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
					{
						if (itTmpQ->isDuplicated(gtmp))
						{
							isExist = true;
							itTmpQ->childIDs.insert(current.code);
							itTmpQ->childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						}
					}

					if (!isExist)
					{
						TreeNode newnode(id);
						newnode.graph = gtmp;
						newnode.graph.idGraph = id;
						newnode.ignoreList = current.ignoreList;
						++id;
						newnode.childIDs.insert(current.code);
						newnode.childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						tmpQ.push_back(newnode);
					}
				}
			}
			
			mainQ.pop_front();
		}

		if (tmpQ.size() > 0)
		{	
			//add new graphs to hashtable
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				itTmpQ->computeDFSCode();
				Hashtable::iterator findIT = table.find(itTmpQ->code);
				if (findIT != table.end())
				{
					itTmpQ->isSavedToTable = true;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
				}
				else
				{
					itTmpQ->isSavedToTable = false;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs, itTmpQ->ignoreList);
				}
			}

			// compute frequency
			for (Hashtable::iterator ht = table.begin(); ht != table.end(); ++ht)
			{
				ht->second.computeFrequency();
			}

			// check which candidates are able to extend in tmpQ
			int count = 0;
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				Hashtable::iterator itFind = table.find(itTmpQ->code);
				if (itFind != table.end())
				{
					if (itFind->second.freq < theta)
					{
						itTmpQ->isStop = true;
						table.erase(itFind);
						++count;
					}
				}
				else
				{
					itTmpQ->isStop = true;
					++count;
				}
			}

			if (count == tmpQ.size())
			{
				stop = true;
			}
			else
			{
				for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
				{
					if (itTmpQ->isStop == false && itTmpQ->isSavedToTable == false)
					{
						Hashtable::iterator itFind = table.find(itTmpQ->code);
						// Compute Correlated value
						for (Hashtable::iterator iht = table.begin(); iht != itFind; ++iht)
						{
							bool isChild = false;
							set<DFSCode>::const_iterator got = itFind->second.childIDs.find(iht->first);
							if (got != itFind->second.childIDs.end())
							{
								isChild = true;
							}
								
							if (isChild == false)
							{
								got = iht->second.childIDs.find(itTmpQ->code);
								if (got != iht->second.childIDs.end())
								{
									isChild = true;
								}
							}
			
							if (isChild == false && Utility::isIgnore(itFind->second, iht->second) == false)
							{
								colocated = 0;
								confidence = 0;
								table.computeCorrelatedValueClose(graph, itFind->second, iht->second, colocated, confidence, hop, numTestHHop);

								if (colocated >= theta && confidence >= phi)
								{
									++countPair;
									CorrelatedResult res;
									res.g1 = itFind->second.graphs[0];
									res.g2 = iht->second.graphs[0];
									res.colocatedvalue = colocated;
									res.confidencevalue = confidence;
									saveResult.insert(res);
								}
							}
						}
					}
				}

				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}

	ofstream of;
	of.open(filenameOuput);
	saveResult.print(of);
	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}

void CorrelatedGraph::ImprovedComputeCorrelatedGraph(char * filenameOuput, int theta, double phi, int hop, int k)
{
	TopKQueue saveResult(k);

	cout << "Building hashtable....." << endl;
	double id = 0;
	deque<TreeNode> mainQ;
	int numTestHHop = 0;
	int colocated = 0;
	double confidence = 0;
	int countPair = 0;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id);
		node.graph.directed = this->directed;
		node.graph.insertVertex(graph[i]);
		node.graph.idGraph = id;
		++id;
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
		Hashtable::iterator findIT = table.find(it->code);
		if (findIT != table.end())
		{
			it->isSavedToTable = true;
		}
		else
		{
			it->isSavedToTable = false;
		}
		table.push(it->code, it->graph);
	}

	//compute frequence of instances in the hashtable
	for (Hashtable::iterator it = table.begin(); it != table.end(); ++it)
	{
		it->second.computeFrequency();
	}

	// remove all graphs being less frequent from queue
	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		Hashtable::iterator itTable = table.find(it->code);
		if (itTable != table.end())
		{
			if (itTable->second.freq < theta)
			{
				it->isStop = true;
				table.erase(itTable);
			}
			else
			{
				if (it->isSavedToTable == false)
				{
					// Compute Correlated value
					for (Hashtable::iterator iht = table.begin(); iht != itTable; ++iht)
					{
						colocated = 0;
						confidence = 0;
						table.computeCorrelatedValueClose(graph, itTable->second, iht->second, colocated, confidence, hop, numTestHHop);

						if (colocated >= theta && confidence >= phi)
						{
							++countPair;
							CorrelatedResult res;
							res.g1 = itTable->second.graphs[0];
							res.g2 = iht->second.graphs[0];
							res.colocatedvalue = colocated;
							res.confidencevalue = confidence;
							saveResult.insert(res);
							//write(of, itTable->second.graphs[0], iht->second.graphs[0], countPair, colocated, confidence);
						}
					}
				}

				for (vector<Graph>::iterator gg = itTable->second.graphs.begin(); gg != itTable->second.graphs.end(); ++gg)
				{
					if (gg->idGraph == it->graph.idGraph)
					{
						it->graph.sameHHop = gg->sameHHop;
						break;
					}
				}
			}
		}
		else
		{
			it->isStop = true;
		}
	}
	bool stop = false;

	deque<TreeNode> tmpQ;

	while (!stop)
	{
		while (!mainQ.empty())
		{
			TreeNode current = mainQ.front();

			if (!current.isStop)
			{
				Hashtable::iterator iht = table.find(current.code);
				current.ignoreList = iht->second.ignoreList;

				// Find current h-hop of current node
				/*Hashtable::iterator iht = table.find(current.code);
				for (vector<Graph>::iterator gg = iht->second.graphs.begin(); gg != iht->second.graphs.end(); ++gg)
				{
					if (gg->idGraph == current.graph.idGraph)
					{
						current.graph.sameHHop = gg->sameHHop;
						break;
					}
				}*/

				// find all neighbouring edges of the graph in current node
				vector<Edge> edges;
				for (vector<Vertex>::iterator itG = current.graph.begin(); itG != current.graph.end(); ++itG)
				{
					int idNode = graph.index(itG->id);
					for (Vertex::edge_iterator itE = graph[idNode].edge.begin(); itE != graph[idNode].edge.end(); itE++)
					{
						if (!current.graph.isExist(*itE) && !Utility::isExistEdgeInList(edges, *itE))
						{
							// add new edge into the candidate set
							edges.push_back(*itE);
						}
					}
				}

				// Create new graph from the candidate set of edges
				for (int ii = 0; ii < edges.size(); ii++)
				{
					Graph gtmp = current.graph;
					gtmp.directed = this->directed;
					gtmp.insertEdge(edges[ii], graph[graph.index(edges[ii].from)].label, graph[graph.index(edges[ii].to)].label);
				
					// Check new graph exists in tmpQ or not?
					bool isExist = false;
					for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
					{
						if (itTmpQ->isDuplicated(gtmp))
						{
							isExist = true;
							itTmpQ->childIDs.insert(current.code);
							itTmpQ->childIDs.insert(current.childIDs.begin(), current.childIDs.end());

							itTmpQ->graph.sameHHop.insert(current.graph.sameHHop.begin(), current.graph.sameHHop.end());
						}
					}

					if (!isExist)
					{
						TreeNode newnode(id);
						newnode.graph = gtmp;
						newnode.graph.idGraph = id;
						++id;
						newnode.ignoreList = current.ignoreList;
						newnode.childIDs.insert(current.code);
						newnode.childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						newnode.graph.sameHHop.insert(current.graph.sameHHop.begin(), current.graph.sameHHop.end());
						tmpQ.push_back(newnode);
					}
				}
			}
			
			mainQ.pop_front();
		}

		if (tmpQ.size() > 0)
		{	
			//add new graphs to hashtable
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				itTmpQ->computeDFSCode();
				Hashtable::iterator findIT = table.find(itTmpQ->code);
				if (findIT != table.end())
				{
					itTmpQ->isSavedToTable = true;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
				}
				else
				{
					itTmpQ->isSavedToTable = false;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs, itTmpQ->ignoreList);
				}
			}

			// compute frequency
			for (Hashtable::iterator ht = table.begin(); ht != table.end(); ++ht)
			{
				ht->second.computeFrequency();
			}

			// check which candidates are able to extend in tmpQ
			int count = 0;
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				Hashtable::iterator itFind = table.find(itTmpQ->code);
				if (itFind != table.end())
				{
					if (itFind->second.freq < theta)
					{
						itTmpQ->isStop = true;
						table.erase(itFind);
						++count;
					}
				}
				else
				{
					itTmpQ->isStop = true;
					++count;
				}
			}

			if (count == tmpQ.size())
			{
				stop = true;
			}
			else
			{
				for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
				{
					if (itTmpQ->isStop == false && itTmpQ->isSavedToTable == false)
					{
						Hashtable::iterator itFind = table.find(itTmpQ->code);
						// Compute Correlated value
						for (Hashtable::iterator iht = table.begin(); iht != itFind; ++iht)
						{
							bool isChild = false;
							set<DFSCode>::const_iterator got = itFind->second.childIDs.find(iht->first);
							if (got != itFind->second.childIDs.end())
							{
								isChild = true;
							}
								
							if (isChild == false)
							{
								got = iht->second.childIDs.find(itTmpQ->code);
								if (got != iht->second.childIDs.end())
								{
									isChild = true;
								}
							}
			
							if (isChild == false && Utility::isIgnore(itFind->second, iht->second) == false)
							{
								colocated = 0;
								confidence = 0;
								table.computeCorrelatedValueClose(graph, itFind->second, iht->second, colocated, confidence, hop, numTestHHop);

								if (colocated >= theta && confidence >= phi)
								{
									++countPair;
									CorrelatedResult res;
									res.g1 = itFind->second.graphs[0];
									res.g2 = iht->second.graphs[0];
									res.colocatedvalue = colocated;
									res.confidencevalue = confidence;
									saveResult.insert(res);
								}
							}
						}
					}
				}

				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}

	ofstream of;
	of.open(filenameOuput);

	saveResult.print(of);

	//of << "Num pairs: " << countPair << endl;
	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}

void CorrelatedGraph::mineCorrelatedGraphFromHashTable(char * filenameOutput, int thetaThres, double phiThres, int hop, int k)
{
	std::priority_queue<CorrelatedResult> saveResult;

	cout << "Computing correlated values....." << endl;
	ofstream of;
	of.open(filenameOutput);
	int count = 0;
	int numTestHHop = 0;

	Hashtable::iterator it1, it2;

	it1 = table.begin();
	++it1;

	for (; it1 != table.end(); ++it1)
	{
		for (it2 = table.begin(); it2 != it1; ++it2)
		{
			bool isChild = false;
			
			for (set<DFSCode>::iterator ch = it1->second.childIDs.begin(); ch != it1->second.childIDs.end(); ++ch)
			{
				if (*ch == it2->first)
				{
					isChild = true;
					break;
				}
			}

			if (isChild == false)
			{
				for (set<DFSCode>::iterator ch = it2->second.childIDs.begin(); ch != it2->second.childIDs.end(); ++ch)
				{
					if (*ch == it1->first)
					{
						isChild = true;
						break;
					}
				}
			}
			
			if (isChild == false)
			{
				// compute correlated value
				int colocated;
				double confidence;

				table.computeCorrelatedValue(graph, it1->second, it2->second, colocated, confidence, hop, numTestHHop);

				if (colocated >= thetaThres && confidence >= phiThres)
				{
					++count;
					//write(of, it1->second.graphs[0], it2->second.graphs[0], count, colocated, confidence);
					CorrelatedResult res;
					res.g1 = it1->second.graphs[0];
					res.g2 = it2->second.graphs[0];
					res.colocatedvalue = colocated;
					res.confidencevalue = confidence;
					saveResult.push(res);
				}
			}
		}
	}

	int num = 0;
	while (!saveResult.empty())
	{
		CorrelatedResult top = saveResult.top();
		top.report_result(of, num + 1);
		saveResult.pop();
		++num;
		if (num >= k)
			break;
	}

	//of << "Num pairs: " << count << endl;
	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}

void CorrelatedGraph::write (ofstream & of, Graph& g1, Graph& g2, int id, double colocated, double confidence)
{
	of << "Pair " << id << endl;
	of << "Co-located value: " << colocated << endl;
	of << "Confidence value: " << confidence << endl;
	of << "G1: " << endl;
	g1.write(of);
	of << "G2: " << endl;
	g2.write(of);
	of << endl;
}

void CorrelatedGraph::baseLineInducedSubgraph(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, int k)
{
	cout << "Running baseline for Induced Subgraphs" << endl;
	this->directed = directed;
	graph.directed = directed;
	initGraph(filenameInput);
	clock_t start, end;

	start = clock();
	computeCorrelatedValueBaselineInducedSubgraph(filenameOuput, theta, phi, hop, k);
	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;
	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::computeCorrelatedValueBaselineInducedSubgraph(char* filenameOuput, int theta, double phi, int hop, int k)
{
	TopKQueue saveResult(k);
	
	double id = 0;
	deque<TreeNode> mainQ;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id);
		node.graph.directed = this->directed;
		node.graph.insertVertex(graph[i]);
		node.graph.idGraph = id;
		++id;
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
		Hashtable::iterator findIT = table.find(it->code);
		if (findIT != table.end())
		{
			it->isSavedToTable = true;
		}
		else
		{
			it->isSavedToTable = false;
		}
		table.push(it->code, it->graph);
	}

	//compute frequence of instances in the hashtable
	for (Hashtable::iterator it = table.begin(); it != table.end(); ++it)
	{
		it->second.computeFrequency();
	}

	int colocated;
	double confidence;
	int countPair = 0;
	int numTestHHop = 0;

	// remove all graphs being less frequent from queue
	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		Hashtable::iterator itTable = table.find(it->code);
		if (itTable != table.end())
		{
			if (itTable->second.freq < theta)
			{
				it->isStop = true;
				table.erase(itTable);
			}
			else
			{
				if (it->isSavedToTable == false)
				{
					// Compute Correlated value
					for (Hashtable::iterator iht = table.begin(); iht != itTable; ++iht)
					{
						colocated = 0;
						confidence = 0;
						table.computeCorrelatedValueClose(graph, itTable->second, iht->second, colocated, confidence, hop, numTestHHop);

						if (colocated >= theta && confidence >= phi)
						{
							++countPair;
							CorrelatedResult res;
							res.g1 = itTable->second.graphs[0];
							res.g2 = iht->second.graphs[0];
							res.colocatedvalue = colocated;
							res.confidencevalue = confidence;
							saveResult.insert(res);
							//write(of, itTable->second.graphs[0], iht->second.graphs[0], countPair, colocated, confidence);
						}
					}
				}
			}
		}
		else
		{
			it->isStop = true;
		}
	}
	bool stop = false;

	deque<TreeNode> tmpQ;

	while (!stop)
	{
		while (!mainQ.empty())
		{
			TreeNode current = mainQ.front();

			if (!current.isStop)
			{
				Hashtable::iterator iht = table.find(current.code);
				current.ignoreList = iht->second.ignoreList;

				// find all neighbouring nodes of the graph in current node
				vector<Vertex> vertices;
				for (vector<Vertex>::iterator itG = current.graph.begin(); itG != current.graph.end(); ++itG)
				{
					int idNode = graph.index(itG->id);
					for (Vertex::edge_iterator itE = graph[idNode].edge.begin(); itE != graph[idNode].edge.end(); itE++)
					{
						Vertex candidate = graph[graph.index(itE->to)];
						if (!current.graph.isExisedVertice(candidate) && !Utility::isExistVertexInList(vertices, candidate))
						{
							// add new vertex into the candidate set
							vertices.push_back(candidate);
						}
					}
				}

				// Create new graph from the candidate set of nodes
				for (int ii = 0; ii < vertices.size(); ii++)
				{
					Graph gtmp = current.graph;
					gtmp.directed = this->directed;
					gtmp.extendByVertex(this->graph, vertices[ii]);
					// Check new graph exists in tmpQ or not?
					bool isExist = false;
					for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
					{
						if (itTmpQ->isDuplicated(gtmp))
						{
							isExist = true;
							itTmpQ->childIDs.insert(current.code);
							itTmpQ->childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						}
					}

					if (!isExist)
					{
						TreeNode newnode(id);
						newnode.graph = gtmp;
						newnode.graph.idGraph = id;
						newnode.ignoreList = current.ignoreList;
						++id;
						newnode.childIDs.insert(current.code);
						newnode.childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						tmpQ.push_back(newnode);
					}
				}
			}
			
			mainQ.pop_front();
		}

		if (tmpQ.size() > 0)
		{	
			//add new graphs to hashtable
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				itTmpQ->computeDFSCode();
				Hashtable::iterator findIT = table.find(itTmpQ->code);
				if (findIT != table.end())
				{
					itTmpQ->isSavedToTable = true;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
				}
				else
				{
					itTmpQ->isSavedToTable = false;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs, itTmpQ->ignoreList);
				}
			}

			// compute frequency
			for (Hashtable::iterator ht = table.begin(); ht != table.end(); ++ht)
			{
				ht->second.computeFrequency();
			}

			// check which candidates are able to extend in tmpQ
			int count = 0;
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				Hashtable::iterator itFind = table.find(itTmpQ->code);
				if (itFind != table.end())
				{
					if (itFind->second.freq < theta)
					{
						itTmpQ->isStop = true;
						table.erase(itFind);
						++count;
					}
				}
				else
				{
					itTmpQ->isStop = true;
					++count;
				}
			}

			if (count == tmpQ.size())
			{
				stop = true;
			}
			else
			{
				for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
				{
					if (itTmpQ->isStop == false && itTmpQ->isSavedToTable == false)
					{
						Hashtable::iterator itFind = table.find(itTmpQ->code);
						// Compute Correlated value
						for (Hashtable::iterator iht = table.begin(); iht != itFind; ++iht)
						{
							bool isChild = false;
							set<DFSCode>::const_iterator got = itFind->second.childIDs.find(iht->first);
							if (got != itFind->second.childIDs.end())
							{
								isChild = true;
							}
								
							if (isChild == false)
							{
								got = iht->second.childIDs.find(itTmpQ->code);
								if (got != iht->second.childIDs.end())
								{
									isChild = true;
								}
							}
			
							if (isChild == false && Utility::isIgnore(itFind->second, iht->second) == false)
							{
								colocated = 0;
								confidence = 0;
								table.computeCorrelatedValueClose(graph, itFind->second, iht->second, colocated, confidence, hop, numTestHHop);

								if (colocated >= theta && confidence >= phi)
								{
									++countPair;
									CorrelatedResult res;
									res.g1 = itFind->second.graphs[0];
									res.g2 = iht->second.graphs[0];
									res.colocatedvalue = colocated;
									res.confidencevalue = confidence;
									saveResult.insert(res);
								}
							}
						}
					}
				}

				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}

	ofstream of;
	of.open(filenameOuput);
	saveResult.print(of);
	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}

void CorrelatedGraph::forwardPruningInducedSubgraph(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, int k)
{
	cout << "Running forward pruning for Induced Subgraphs" << endl;
	this->directed = directed;
	graph.directed = directed;
	initGraph(filenameInput);
	clock_t start, end;

	start = clock();
	computeCorrelatedForwardPruningInducedSubgraph(filenameOuput, theta, phi, hop, k);
	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;
	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::computeCorrelatedForwardPruningInducedSubgraph(char* filenameOuput, int theta, double phi, int hop, int k)
{
	TopKQueue saveResult(k);

	double id = 0;
	deque<TreeNode> mainQ;
	int numTestHHop = 0;
	int colocated = 0;
	double confidence = 0;
	int countPair = 0;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id);
		node.graph.directed = this->directed;
		node.graph.insertVertex(graph[i]);
		node.graph.idGraph = id;
		++id;
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
		Hashtable::iterator findIT = table.find(it->code);
		if (findIT != table.end())
		{
			it->isSavedToTable = true;
		}
		else
		{
			it->isSavedToTable = false;
		}
		table.push(it->code, it->graph);
	}

	//compute frequence of instances in the hashtable
	for (Hashtable::iterator it = table.begin(); it != table.end(); ++it)
	{
		it->second.computeFrequency();
	}

	// remove all graphs being less frequent from queue
	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		Hashtable::iterator itTable = table.find(it->code);
		if (itTable != table.end())
		{
			if (itTable->second.freq < theta)
			{
				it->isStop = true;
				table.erase(itTable);
			}
			else
			{
				if (it->isSavedToTable == false)
				{
					// Compute Correlated value
					for (Hashtable::iterator iht = table.begin(); iht != itTable; ++iht)
					{
						colocated = 0;
						confidence = 0;
						table.computeCorrelatedValueClose(graph, itTable->second, iht->second, colocated, confidence, hop, numTestHHop);

						if (colocated >= theta && confidence >= phi)
						{
							++countPair;
							CorrelatedResult res;
							res.g1 = itTable->second.graphs[0];
							res.g2 = iht->second.graphs[0];
							res.colocatedvalue = colocated;
							res.confidencevalue = confidence;
							saveResult.insert(res);
							//write(of, itTable->second.graphs[0], iht->second.graphs[0], countPair, colocated, confidence);
						}
					}
				}

				for (vector<Graph>::iterator gg = itTable->second.graphs.begin(); gg != itTable->second.graphs.end(); ++gg)
				{
					if (gg->idGraph == it->graph.idGraph)
					{
						it->graph.sameHHop = gg->sameHHop;
						break;
					}
				}
			}
		}
		else
		{
			it->isStop = true;
		}
	}
	bool stop = false;

	deque<TreeNode> tmpQ;

	while (!stop)
	{
		while (!mainQ.empty())
		{
			TreeNode current = mainQ.front();

			if (!current.isStop)
			{
				Hashtable::iterator iht = table.find(current.code);
				current.ignoreList = iht->second.ignoreList;

				// find all neighbouring nodes of the graph in current node
				vector<Vertex> vertices;
				for (vector<Vertex>::iterator itG = current.graph.begin(); itG != current.graph.end(); ++itG)
				{
					int idNode = graph.index(itG->id);
					for (Vertex::edge_iterator itE = graph[idNode].edge.begin(); itE != graph[idNode].edge.end(); itE++)
					{
						Vertex candidate = graph[graph.index(itE->to)];
						if (!current.graph.isExisedVertice(candidate) && !Utility::isExistVertexInList(vertices, candidate))
						{
							// add new vertex into the candidate set
							vertices.push_back(candidate);
						}
					}
				}

				// Create new graph from the candidate set of nodes
				for (int ii = 0; ii < vertices.size(); ii++)
				{
					Graph gtmp = current.graph;
					gtmp.directed = this->directed;
					gtmp.extendByVertex(this->graph, vertices[ii]);
					// Check new graph exists in tmpQ or not?
					bool isExist = false;
					for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
					{
						if (itTmpQ->isDuplicated(gtmp))
						{
							isExist = true;
							itTmpQ->childIDs.insert(current.code);
							itTmpQ->childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						}
					}

					if (!isExist)
					{
						TreeNode newnode(id);
						newnode.graph = gtmp;
						newnode.graph.idGraph = id;
						newnode.ignoreList = current.ignoreList;
						++id;
						newnode.childIDs.insert(current.code);
						newnode.childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						tmpQ.push_back(newnode);
					}
				}
			}
			
			mainQ.pop_front();
		}

		if (tmpQ.size() > 0)
		{	
			//add new graphs to hashtable
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				itTmpQ->computeDFSCode();
				Hashtable::iterator findIT = table.find(itTmpQ->code);
				if (findIT != table.end())
				{
					itTmpQ->isSavedToTable = true;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
				}
				else
				{
					itTmpQ->isSavedToTable = false;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs, itTmpQ->ignoreList);
				}
			}

			// compute frequency
			for (Hashtable::iterator ht = table.begin(); ht != table.end(); ++ht)
			{
				ht->second.computeFrequency();
			}

			// check which candidates are able to extend in tmpQ
			int count = 0;
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				Hashtable::iterator itFind = table.find(itTmpQ->code);
				if (itFind != table.end())
				{
					if (itFind->second.freq < theta)
					{
						itTmpQ->isStop = true;
						table.erase(itFind);
						++count;
					}
				}
				else
				{
					itTmpQ->isStop = true;
					++count;
				}
			}

			if (count == tmpQ.size())
			{
				stop = true;
			}
			else
			{
				for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
				{
					if (itTmpQ->isStop == false && itTmpQ->isSavedToTable == false)
					{
						Hashtable::iterator itFind = table.find(itTmpQ->code);
						// Compute Correlated value
						for (Hashtable::iterator iht = table.begin(); iht != itFind; ++iht)
						{
							bool isChild = false;
							set<DFSCode>::const_iterator got = itFind->second.childIDs.find(iht->first);
							if (got != itFind->second.childIDs.end())
							{
								isChild = true;
							}
								
							if (isChild == false)
							{
								got = iht->second.childIDs.find(itTmpQ->code);
								if (got != iht->second.childIDs.end())
								{
									isChild = true;
								}
							}
			
							if (isChild == false && Utility::isIgnore(itFind->second, iht->second) == false)
							{
								colocated = 0;
								confidence = 0;
								table.computeCorrelatedValueClose(graph, itFind->second, iht->second, colocated, confidence, hop, numTestHHop);

								if (colocated >= theta && confidence >= phi)
								{
									++countPair;
									CorrelatedResult res;
									res.g1 = itFind->second.graphs[0];
									res.g2 = iht->second.graphs[0];
									res.colocatedvalue = colocated;
									res.confidencevalue = confidence;
									saveResult.insert(res);
								}
							}
						}
					}
				}

				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}

	ofstream of;
	of.open(filenameOuput);

	saveResult.print(of);

	//of << "Num pairs: " << countPair << endl;
	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}

void CorrelatedGraph::topKPruningInducedSubgraph(bool directed, char * filenameInput, char * filenameOuput, int theta, double phi, int hop, int k)
{
	cout << "Running top-K pruning for Induced Subgraphs" << endl;
	this->directed = directed;
	graph.directed = directed;
	initGraph(filenameInput);
	clock_t start, end;

	start = clock();
	computeCorrelatedTopKPruningInducedSubgraph(filenameOuput, theta, phi, hop, k);
	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;
	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::computeCorrelatedTopKPruningInducedSubgraph(char* filenameOuput, int theta, double phi, int hop, int k)
{
	TopKQueue topKqueue(k);

	double id = 0;
	deque<TreeNode> mainQ;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id);
		node.graph.directed = this->directed;
		node.graph.insertVertex(graph[i]);
		node.graph.idGraph = id;
		++id;
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
		Hashtable::iterator findIT = table.find(it->code);
		if (findIT != table.end())
		{
			it->isSavedToTable = true;
		}
		else
		{
			it->isSavedToTable = false;
		}
		table.push(it->code, it->graph);
	}

	//compute frequence of instances in the hashtable
	for (Hashtable::iterator it = table.begin(); it != table.end(); ++it)
	{
		it->second.computeFrequency();
	}
	int colocated;
	double confidence;
	int countPair = 0;
	int numTestHHop = 0;

	// remove all graphs being less frequent from queue
	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		Hashtable::iterator itTable = table.find(it->code);
		if (itTable != table.end())
		{
			if (itTable->second.freq < theta || (topKqueue.isFull() &&  itTable->second.freq < topKqueue.minCorrelatedValue()))
			{
				it->isStop = true;
				table.erase(itTable);
			}
			else
			{
				if (it->isSavedToTable == false)
				{
					// Compute Correlated value
					for (Hashtable::iterator iht = table.begin(); iht != itTable; ++iht)
					{
						colocated = 0;
						confidence = 0;
						table.computeCorrelatedValueClose(graph, itTable->second, iht->second, colocated, confidence, hop, numTestHHop);

						if (colocated >= theta && confidence >= phi)
						{
							++countPair;
							CorrelatedResult res;
							res.g1 = itTable->second.graphs[0];
							res.g2 = iht->second.graphs[0];
							res.colocatedvalue = colocated;
							res.confidencevalue = confidence;
							topKqueue.insert(res);
							//write(of, itTable->second.graphs[0], iht->second.graphs[0], countPair, colocated, confidence);
						}
					}
				}
			}
		}
		else
		{
			it->isStop = true;
		}
	}
	bool stop = false;

	deque<TreeNode> tmpQ;

	while (!stop)
	{
		while (!mainQ.empty())
		{
			TreeNode current = mainQ.front();

			if (!current.isStop)
			{
				Hashtable::iterator iht = table.find(current.code);
				current.ignoreList = iht->second.ignoreList;

				// find all neighbouring nodes of the graph in current node
				vector<Vertex> vertices;
				for (vector<Vertex>::iterator itG = current.graph.begin(); itG != current.graph.end(); ++itG)
				{
					int idNode = graph.index(itG->id);
					for (Vertex::edge_iterator itE = graph[idNode].edge.begin(); itE != graph[idNode].edge.end(); itE++)
					{
						Vertex candidate = graph[graph.index(itE->to)];
						if (!current.graph.isExisedVertice(candidate) && !Utility::isExistVertexInList(vertices, candidate))
						{
							// add new vertex into the candidate set
							vertices.push_back(candidate);
						}
					}
				}

				// Create new graph from the candidate set of nodes
				for (int ii = 0; ii < vertices.size(); ii++)
				{
					Graph gtmp = current.graph;
					gtmp.directed = this->directed;
					gtmp.extendByVertex(this->graph, vertices[ii]);
					// Check new graph exists in tmpQ or not?
					bool isExist = false;
					for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
					{
						if (itTmpQ->isDuplicated(gtmp))
						{
							isExist = true;
							itTmpQ->childIDs.insert(current.code);
							itTmpQ->childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						}
					}

					if (!isExist)
					{
						TreeNode newnode(id);
						newnode.graph = gtmp;
						newnode.graph.idGraph = id;
						newnode.ignoreList = current.ignoreList;
						++id;
						newnode.childIDs.insert(current.code);
						newnode.childIDs.insert(current.childIDs.begin(), current.childIDs.end());
						tmpQ.push_back(newnode);
					}
				}
			}
			
			mainQ.pop_front();
		}

		if (tmpQ.size() > 0)
		{	
			//add new graphs to hashtable
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				itTmpQ->computeDFSCode();
				Hashtable::iterator findIT = table.find(itTmpQ->code);
				if (findIT != table.end())
				{
					itTmpQ->isSavedToTable = true;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
				}
				else
				{
					itTmpQ->isSavedToTable = false;
					table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs, itTmpQ->ignoreList);
				}
			}

			// compute frequency
			for (Hashtable::iterator ht = table.begin(); ht != table.end(); ++ht)
			{
				ht->second.computeFrequency();
			}

			// check which candidates are able to extend in tmpQ
			int count = 0;
			for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
			{
				Hashtable::iterator itFind = table.find(itTmpQ->code);
				if (itFind != table.end())
				{
					if (itFind->second.freq < theta || (topKqueue.isFull() &&  itFind->second.freq < topKqueue.minCorrelatedValue()))
					{
						itTmpQ->isStop = true;
						table.erase(itFind);
						++count;
					}
				}
				else
				{
					itTmpQ->isStop = true;
					++count;
				}
			}

			if (count == tmpQ.size())
			{
				stop = true;
			}
			else
			{
				for (deque<TreeNode>::iterator itTmpQ = tmpQ.begin(); itTmpQ != tmpQ.end(); ++itTmpQ)
				{
					if (itTmpQ->isStop == false && itTmpQ->isSavedToTable == false)
					{
						Hashtable::iterator itFind = table.find(itTmpQ->code);
						// Compute Correlated value
						for (Hashtable::iterator iht = table.begin(); iht != itFind; ++iht)
						{
							bool isChild = false;
							set<DFSCode>::const_iterator got = itFind->second.childIDs.find(iht->first);
							if (got != itFind->second.childIDs.end())
							{
								isChild = true;
							}
								
							if (isChild == false)
							{
								got = iht->second.childIDs.find(itTmpQ->code);
								if (got != iht->second.childIDs.end())
								{
									isChild = true;
								}
							}
			
							if (isChild == false && Utility::isIgnore(itFind->second, iht->second) == false)
							{
								colocated = 0;
								confidence = 0;
								table.computeCorrelatedValueClose(graph, itFind->second, iht->second, colocated, confidence, hop, numTestHHop);

								if (colocated >= theta && confidence >= phi)
								{
									++countPair;
									CorrelatedResult res;
									res.g1 = itFind->second.graphs[0];
									res.g2 = iht->second.graphs[0];
									res.colocatedvalue = colocated;
									res.confidencevalue = confidence;
									topKqueue.insert(res);
								}
							}
						}
					}
				}

				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}

	ofstream of;
	of.open(filenameOuput);
	topKqueue.print(of);

	of << "No. call to H-hop Test: " << numTestHHop;
	of.close();
}