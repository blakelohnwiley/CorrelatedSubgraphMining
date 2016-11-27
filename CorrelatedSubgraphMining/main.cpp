#include <iostream>
#include <fstream>
#include "graph.h"

using namespace std;

int main (int argc, char * const  argv[]) {
	
	ifstream is;

	char* fname;
	fname = "graph.txt";
	char* fname2;
	fname2 = "graph1.txt";

	Graph graph;
	graph.read(fname);

	Graph graph1;
	graph1.read(fname2);

	graph1.check();
	cin.get();
	return 0;
}