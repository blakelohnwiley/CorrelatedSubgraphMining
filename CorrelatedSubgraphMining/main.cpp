#include <iostream>
#include <fstream>
#include <fstream>
#include <cstring>
#include "graph.h"
#include "graphtomindfscode.h"
#include "overlapgraph.h"

using namespace std;

int main (int argc, char * const  argv[]) {
	
	ifstream is;

	char* fname;
	fname = "graph2.txt";
	char* fname1;
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

	/*DFSCode dfsCode;
	GraphToMinDFSCode convertGraphToDFSCode;
	convertGraphToDFSCode.findMinimumDFSCode(&graph1, dfsCode);
	dfsCode.write(cout);*/

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

	cin.get();
	return 0;
}