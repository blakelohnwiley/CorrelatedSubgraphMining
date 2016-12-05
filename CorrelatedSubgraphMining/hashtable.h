#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "graph.h"
#include "graphcode.h"
#include "instance.h"
#include "utility.h"
#include <map>
#include <string>

using namespace std;

class Hashtable : public map<DFSCode, Instance>
{
public:
	typedef map<DFSCode, Instance>::iterator hastable_iterator;

	void push(DFSCode& dfs_code, Graph& g, set<DFSCode>& child);
	void push(DFSCode& dfs_code, Graph& g);
	void computeFrequency();
	void computeFrequency(Hashtable::hastable_iterator start);
	int freq(DFSCode dfs_code);
	void computeCorrelatedValue (Graph & bigGraph, Instance & ins1, Instance & ins2, int & colocated, double & confidence, int hop, int& numTestCollocated);
	void computeCorrelatedValueClose (Graph & bigGraph, Instance& ins1, Instance& ins2, int & colocated, double & confidence, int hop, int& numTestCollocated);
};

#endif