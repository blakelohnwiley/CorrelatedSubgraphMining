#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "graph.h"
#include "graphcode.h"
#include "instance.h"
#include "utility.h"
#include <map>
#include <string>

using namespace std;

class Hashtable : public map<string, Instance>
{
public:
	typedef map<string, Instance>::iterator hastable_iterator;

	void push(string& dfs_code, Graph& g, set<string>& child);
	void push(string& dfs_code, Graph& g);
	void computeFrequency();
	void computeFrequency(Hashtable::hastable_iterator start);
	int freq(string dfs_code);
	void computeCorrelatedValue (Graph & bigGraph, Instance & ins1, Instance & ins2, int & colocated, double & confidence, int hop, int& numTestCollocated);
};

#endif