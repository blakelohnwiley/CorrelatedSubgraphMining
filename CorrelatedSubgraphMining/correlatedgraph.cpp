#include "correlatedgraph.h"
#include <time.h>

void CorrelatedGraph::initGraph(char * filename)
{
	graph.read(filename);
}

void CorrelatedGraph::baseLine(char * filenameInput, char * filenameOuput, int theta, double phi, int hop)
{
	cout << "Running......." << endl;

	initGraph(filenameInput);
	time_t start, end;

	time(&start);
	constructHashTable(theta, phi, hop);
	mineCorrelatedGraphFromHashTable(filenameOuput, theta, phi, hop);
	time(&end);

	double duration = difftime(end, start);
	ofstream of;
	of.open(filenameOuput, std::ios::app);
	of << endl << "Finished! in " << duration << " (s)";
	of.close();

	cout << "Finished! in " << duration << " (s)";
}

void CorrelatedGraph::constructHashTable(double theta, double phi, double hop)
{
	//ofstream of;
	//of.open("result1.txt");
	cout << "Building hashtable....." << endl;
	int id = 0;
	deque<TreeNode> mainQ;

	for (int i = 0; i < graph.vertex_size(); i++)
	{
		TreeNode node(id++);
		node.graph.insertVertex(graph[i]);
		mainQ.push_back(node);
	}

	for (deque<TreeNode>::iterator it = mainQ.begin(); it != mainQ.end(); ++it)
	{
		it->computeDFSCode();
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
					gtmp.insertEdge(edges[ii], graph[graph.index(edges[ii].from)].label, graph[graph.index(edges[ii].to)].label);
					/*DFSCode code;
					Utility::computeDFSCode(gtmp, code);*/

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
						TreeNode newnode(id++);
						newnode.graph = gtmp;
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
				table.push(itTmpQ->code, itTmpQ->graph, itTmpQ->childIDs);
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
				mainQ = tmpQ;
				tmpQ.clear();
			}
		}
		else
		{
			stop = true;
		}
	}


	//of.close();
}

void CorrelatedGraph::mineCorrelatedGraphFromHashTable(char * filenameOutput, int thetaThres, double phiThres, int hop)
{
	cout << "Computing correlated values....." << endl;
	ofstream of;
	of.open(filenameOutput);
	int count = 0;
	int numTestHHop = 0;

	Hashtable::iterator it1, it2;
	it1 = table.end();
	--it1;


	for (; it1 != table.begin(); --it1)
	{
		it2 = table.begin();

		for (; it2 != it1; ++it2)
		{
			bool isChild = false;
			
			for (set<string>::iterator ch = it1->second.childIDs.begin(); ch != it1->second.childIDs.end(); ++ch)
			{
				if (*ch == it2->first)
				{
					isChild = true;
					break;
				}
			}

			if (isChild == false)
			{
				for (set<string>::iterator ch = it2->second.childIDs.begin(); ch != it2->second.childIDs.end(); ++ch)
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
					write(of, it1->second.graphs[0], it2->second.graphs[0], count, colocated, confidence);
				}
			}
		}

		//cout << count << " times" << endl;
		//cout << "Finish one element in hashtable" << endl;
	}

	of << "Num pairs: " << count << endl;
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