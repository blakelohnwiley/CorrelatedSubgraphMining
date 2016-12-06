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

	char * configName = "../Data/config.txt";

	Utility::readConfigFile(configName, theta, phi, hop);

	cout << "1. Baseline" << endl;
	cout << "2. Forward Pruning" << endl;
	char c;
	cout << "Choose?: ";
	cin >> c;
	
	if (c == '1')
	{
		CorrelatedGraph correlatedGraph;
		correlatedGraph.baseLine(fname, "../Data/result_baseline.txt", theta, phi, hop);
	}
	else
	{
		CorrelatedGraph correlatedGraph1;
		correlatedGraph1.pruning(fname, "../Data/result_pruning.txt", theta, phi, hop);
	}


	cin.get();
	return 0;
}