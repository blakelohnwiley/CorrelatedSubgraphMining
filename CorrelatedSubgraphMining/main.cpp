#include <iostream>
#include <fstream>
#include "graph.h"
#include "graphtomindfscode.h"

using namespace std;

int main (int argc, char * const  argv[]) {
	
	ifstream is;

	char* fname;
	fname = "graph.txt";
	char* fname2;
	fname2 = "graph1.txt";

	/*Graph graph;
	graph.read(fname);*/

	Graph graph1;
	graph1.read(fname2);

	DFSCode dfsCode;
	GraphToMinDFSCode convertGraphToDFSCode;
	convertGraphToDFSCode.findMinimumDFSCode(&graph1, dfsCode);
	dfsCode.write(cout);

	cin.get();
	return 0;
}