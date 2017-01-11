#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "graph.h"
#include "graphcode.h"
#include "instance.h"
#include "utility.h"
#include <map>
#include <string>
#include <set>
#include <stdint.h>
#include <unordered_set>
#include <unordered_map>

using namespace std;

class Hashtable : public unordered_map<DFSCode, Instance>
{
public:
	typedef unordered_map<DFSCode, Instance>::iterator hastable_iterator;

	void push(DFSCode& dfs_code, Graph& g, unordered_set<DFSCode>& child);
	void push(DFSCode& dfs_code, Graph& g, unordered_set<DFSCode>& child, unordered_set<CodeId> & ignore);
	void push(DFSCode& dfs_code, Graph& g);
	void assign(DFSCode& dfs_code, vector<Graph>& listGraph);
	void assign(DFSCode& dfs_code, vector<Graph>& listGraph, int freq_);
	void assign(DFSCode& dfs_code, vector<Graph>& listGraph, unordered_set<CodeId> & ignore);
	void insertIgnoreList(DFSCode& dfs_code, unordered_set<CodeId> & ignore);
	void computeFrequency();
	void computeFrequency(Hashtable::hastable_iterator start);
	int freq(DFSCode dfs_code);
	void computeCorrelatedValue (Graph & bigGraph, Instance & ins1, Instance & ins2, int & colocated, double & confidence, int hop, int& numTestCollocated);
	void computeCorrelatedValueClose (Graph & bigGraph, Instance& ins1, Instance& ins2, int & colocated, double & confidence, int hop, int& numTestCollocated);
};

#endif