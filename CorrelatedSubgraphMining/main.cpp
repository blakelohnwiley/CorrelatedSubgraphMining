#include "utility.h"
#include "graph.h"
#include "graphtomindfscode.h"
#include "overlapgraph.h"
#include "correlatedgraph.h"
#include "dualiso.h"

#include <iostream>
#include <fstream>
#include <fstream>
#include <cstring>

using namespace std;

void testSubgraphMatching()
{
	char* fname = new char [80];
	strcpy(fname, "../Data/");
	char * name = new char[50];
	cout << "Input the graph file name: ";
	cin.getline(name, 50);
	fname = strcat(fname, name);
	Graph database(true);
	database.read(fname);

	Graph pattern(true);

	Vertex v1;
	v1.id = 0;
	v1.label = 1;
	pattern.insertVertex(v1);
	pattern[0].push(0, 1, 0);

	Vertex v2;
	v2.id = 1;
	v2.label = 2;
	pattern.insertVertex(v2);
	pattern[1].push(1, 0, 0);
	pattern[1].push(1, 2, 0);
	pattern[1].push(1, 3, 0);

	Vertex v3;
	v3.id = 2;
	v3.label = 3;
	pattern.insertVertex(v3);

	Vertex v4;
	v4.id = 3;
	v4.label = 3;
	pattern.insertVertex(v4);
	

	pattern.buildEdge();
	
	DualISO dualISO;
	vector<Graph> res = dualISO.match(database, pattern);

	for (int i = 0; i < res.size(); i++)
	{
		cout << "Result: " << (i + 1) << endl;
		res[i].write(cout);
	}
}

void testGetDownNeighborsExactGraph()
{
	Graph pattern(false);
	Vertex v1;
	v1.id = 0;
	v1.label = 1;
	pattern.insertVertex(v1);
	pattern[0].push(0, 1, 0);
	//pattern[0].push(0, 2, 0);

	Vertex v2;
	v2.id = 1;
	v2.label = 2;
	pattern.insertVertex(v2);
	pattern[1].push(1, 0, 0);
	pattern[1].push(1, 2, 0);

	Vertex v3;
	v3.id = 2;
	v3.label = 3;
	pattern.insertVertex(v3);
	//pattern[2].push(2, 0, 0);
	pattern[2].push(2, 1, 0);
	pattern[2].push(2, 3, 0);

	Vertex v4;
	v4.id = 3;
	v4.label = 1;
	pattern.insertVertex(v4);
	pattern[3].push(3, 2, 0);

	vector<Graph> res1 = pattern.getDownNeighborsExactGraph();
	cout << "Test Result 1: " << endl;
	for (int i = 0; i < res1.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res1[i].write(cout);
	}

	Graph pattern1(true);
	Vertex v5;
	v5.id = 0;
	v5.label = 2;
	pattern1.insertVertex(v5);
	pattern1[0].push(0, 1, 0);
	pattern1[0].push(0, 2, 0);

	Vertex v6;
	v6.id = 1;
	v6.label = 1;
	pattern1.insertVertex(v6);

	Vertex v7;
	v7.id = 2;
	v7.label = 4;
	pattern1.insertVertex(v7);

	Vertex v8;
	v8.id = 3;
	v8.label = 3;
	pattern1.insertVertex(v4);
	pattern1[3].push(3, 2, 0);

	vector<Graph> res2 = pattern1.getDownNeighborsExactGraph();
	cout << "Test Result 2: " << endl;
	for (int i = 0; i < res2.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res2[i].write(cout);
	}

	Graph pattern2(false);
	Vertex u1;
	u1.id = 0;
	u1.label = 1;
	pattern2.insertVertex(u1);
	pattern2[0].push(0, 1, 0);
	pattern2[0].push(0, 2, 0);

	Vertex u2;
	u2.id = 1;
	u2.label = 2;
	pattern2.insertVertex(u2);
	pattern2[1].push(1, 0, 0);

	Vertex u3;
	u3.id = 2;
	u3.label = 3;
	pattern2.insertVertex(u3);
	pattern2[2].push(2, 0, 0);

	vector<Graph> res3 = pattern2.getDownNeighborsExactGraph();
	cout << "Test Result 3: " << endl;
	for (int i = 0; i < res3.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res3[i].write(cout);
	}

	Graph pattern3(true);
	u3.id = 0;
	u3.label = 1;
	pattern3.insertVertex(u3);

	Vertex u4;
	u4.id = 1;
	u4.label = 2;
	pattern3.insertVertex(u4);
	pattern3[1].push(1, 0, 0);

	Vertex u5;
	u5.id = 2;
	u5.label = 3;
	pattern3.insertVertex(u5);
	pattern3[2].push(2, 0, 0);

	vector<Graph> res4 = pattern3.getDownNeighborsExactGraph();
	cout << "Test Result 4: " << endl;
	for (int i = 0; i < res4.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res4[i].write(cout);
	}

	Graph pattern4(false);
	Vertex u6;
	u6.id = 0;
	u6.label = 1;
	pattern4.insertVertex(u6);
	pattern4[0].push(0, 1, 0);

	Vertex u7;
	u7.id = 1;
	u7.label = 2;
	pattern4.insertVertex(u7);
	pattern4[1].push(1, 0, 0);

	vector<Graph> res5 = pattern4.getDownNeighborsExactGraph();
	cout << "Test Result 5: " << endl;
	for (int i = 0; i < res5.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res5[i].write(cout);
	}

	Graph pattern5(true);
	Vertex u8;
	u8.id = 0;
	u8.label = 1;
	pattern5.insertVertex(u8);
	pattern5[0].push(0, 1, 0);

	Vertex u9;
	u9.id = 1;
	u9.label = 2;
	pattern5.insertVertex(u9);

	vector<Graph> res6 = pattern5.getDownNeighborsExactGraph();
	cout << "Test Result 6: " << endl;
	for (int i = 0; i < res6.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res6[i].write(cout);
	}
}

void testGetUpNeighborsExactGraph()
{
	char* fname = new char [80];
	strcpy(fname, "../Data/");
	char * name = "graph2.txt";
	fname = strcat(fname, name);

	Graph graph(false);
	graph.read(fname);

	Graph pattern1(false);
	Vertex u1;
	u1.id = 9;
	u1.label = 1;
	pattern1.insertVertex(u1);
	pattern1[0].push(9, 10, 0);

	Vertex u2;
	u2.id = 10;
	u2.label = 2;
	pattern1.insertVertex(u2);
	pattern1[1].push(10, 9, 0);
	pattern1[1].push(10, 12, 0);

	Vertex u3;
	u3.id = 12;
	u3.label = 3;
	pattern1.insertVertex(u3);
	pattern1[2].push(12, 10, 0);

	vector<Graph> res1 = pattern1.getUpNeigborsExactGraph(graph);
	cout << "Test Result 1: " << endl;
	for (int i = 0; i < res1.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res1[i].write(cout);
	}

	char* fname1 = new char [80];
	strcpy(fname1, "../Data/");
	char * name1 = "graph6.txt";
	fname1 = strcat(fname1, name1);

	Graph graph1(true);
	graph1.read(fname1);

	Graph pattern2(true);
	Vertex v1;
	v1.id = 2;
	v1.label = 1;
	pattern2.insertVertex(v1);
	pattern2[0].push(2, 7, 0);

	Vertex v2;
	v2.id = 7;
	v2.label = 2;
	pattern2.insertVertex(v2);
	pattern2[1].push(7, 6, 0);

	Vertex v3;
	v3.id = 6;
	v3.label = 1;
	pattern2.insertVertex(v3);
	
	vector<Graph> res2 = pattern2.getUpNeigborsExactGraph(graph1);
	cout << "Test Result 2: " << endl;
	for (int i = 0; i < res2.size(); ++i)
	{
		cout << "Graph " << (i + 1) << endl;
		res2[i].write(cout);
	}
}

int main (int argc, char * const  argv[]) {
	
	ifstream is;

	/*char* fname = new char [80];
	strcpy(fname, "../Data/");
	char * name = new char[50];
	cout << "Input the graph file name: ";
	cin.getline(name, 50);
	fname = strcat(fname, name);*/
	/*Graph graph;
	graph.read(fname);

	DFSCode dfsCode;
	GraphToMinDFSCode convertGraphToDFSCode;
	convertGraphToDFSCode.findMinimumDFSCode(&graph, dfsCode);
	dfsCode.write(cout);*/

	/*char* fname1;
	fname1 = "graph3.txt";
	char* fname2;
	fname2 = "graph4.txt";
	char* fname3;
	fname3 = "graph5.txt";

	Graph graph;
	graph.read(fname);

	Graph graph1;
	graph1.read(fname1);

	Graph graph2;
	graph2.read(fname2);

	Graph graph3;
	graph3.read(fname3);

	Graph graph4;
	graph4.read("graph1.txt");

	DFSCode dfsCode;
	GraphToMinDFSCode convertGraphToDFSCode;
	convertGraphToDFSCode.findMinimumDFSCode(&graph4, dfsCode);
	dfsCode.write(cout);

	OverlapGraph ograph(4);
	ograph.add(&graph, 0);
	ograph.add(&graph1, 1);
	ograph.add(&graph2, 2);
	ograph.add(&graph3, 3);

	vector<int> group;
	int t = ograph.getMISSize(group);

	cout << "Num MIS = " << t << endl;
	cout << "group: ";
	for (int i = 0; i < group.size(); i++)
	{
		cout << group[i] << "  ";
	}
*/
	/*int theta;
	double phi;
	int hop;
	int k;

	char * configName = "../Data/config.txt";

	Utility::readConfigFile(configName, theta, phi, hop, k);
	bool directed;

	cout << "---GRAPH DATASET----" << endl;
	cout << "1. Undirected" << endl;
	cout << "2. Directed" << endl;
	cout << "Choose?: ";
	int cc;
	cin >> cc;

	if (cc == 2)
	{
		directed = true;
	}
	else
	{
		directed = false;
	}

	cout << endl << "---ALGORITHM----" << endl;
	cout << "1. Baseline - Exact Sugraphs" << endl;
	cout << "2. Forward Pruning - Exact Subgraphs" << endl;
	cout << "3. Top-K Pruning - Exact Subgraphs" << endl;
	cout << "4. Baseline - Induced Subgraphs" << endl;
	cout << "5. Forward Pruning - Induced Subgraphs" << endl;
	cout << "6. Top-K Pruning - Induced Subgraphs" << endl;
	int c;
	cout << "Choose?: ";
	cin >> c;
	
	if (c == 1)
	{
		CorrelatedGraph correlatedGraph;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_baseline_exact_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph.baseLine(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == 2)
	{
		CorrelatedGraph correlatedGraph1;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_forward_pruning_exact_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph1.forwardPruning(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == 3)
	{
		CorrelatedGraph correlatedGraph2;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_top_k_pruning_exact_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph2.topkPruning(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == 4)
	{
		CorrelatedGraph correlatedGraph3;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_baseline_induced_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph3.baseLineInducedSubgraph(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == 5)
	{
		CorrelatedGraph correlatedGraph4;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_forward_pruning_induced_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph4.forwardPruningInducedSubgraph(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == 6)
	{
		CorrelatedGraph correlatedGraph5;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_topk_pruning_induced_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph5.topKPruningInducedSubgraph(directed, fname, resultfilename, theta, phi, hop, k);
	}*/

	//testGetDownNeighborsExactGraph();
	testGetUpNeighborsExactGraph();

	getchar();
	return 0;
}