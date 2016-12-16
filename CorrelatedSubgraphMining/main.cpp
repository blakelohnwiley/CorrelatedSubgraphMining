#include "utility.h"
#include "graph.h"
#include "graphtomindfscode.h"
#include "overlapgraph.h"
#include "correlatedgraph.h"

#include <iostream>
#include <fstream>
#include <fstream>
#include <cstring>

using namespace std;

int main (int argc, char * const  argv[]) {
	
	ifstream is;

	char* fname = new char [80];
	strcpy(fname, "../Data/");
	char * name = new char[50];
	cout << "Input the graph file name: ";
	cin.getline(name, 50);
	fname = strcat(fname, name);
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
	int theta;
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
	char cc;
	cin >> cc;

	if (cc == '2')
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
	char c;
	cout << "Choose?: ";
	cin >> c;
	
	if (c == '1')
	{
		CorrelatedGraph correlatedGraph;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_baseline_exact_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph.baseLine(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == '2')
	{
		CorrelatedGraph correlatedGraph1;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_forward_pruning_exact_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph1.forwardPruning(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == '3')
	{
		CorrelatedGraph correlatedGraph2;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_top_k_pruning_exact_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph2.topkPruning(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == '4')
	{
		CorrelatedGraph correlatedGraph3;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_baseline_induced_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph3.baseLineInducedSubgraph(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == '5')
	{
		CorrelatedGraph correlatedGraph4;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_forward_pruning_induced_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph4.forwardPruningInducedSubgraph(directed, fname, resultfilename, theta, phi, hop, k);
	}
	else if (c == '6')
	{
		CorrelatedGraph correlatedGraph5;
		char * resultfilename = new char[256];
		strcpy(resultfilename, "../Data/result_topk_pruning_induced_subgraphs_");
		resultfilename = strcat(resultfilename, name);
		correlatedGraph5.topKPruningInducedSubgraph(directed, fname, resultfilename, theta, phi, hop, k);
	}


	cin.get();
	return 0;
}