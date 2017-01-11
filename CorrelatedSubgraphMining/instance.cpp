#include "instance.h"

const Instance & Instance::operator = (const Instance & ins)
{
	if (this != &ins)
	{
		graphs.clear();
		graphs = ins.graphs;
		childIDs.clear();
		childIDs = ins.childIDs;
		freq = ins.freq;
		ignoreList.clear();
		ignoreList = ins.ignoreList;
		mapIdToIndexGraph.clear();
		mapIdToIndexGraph = ins.mapIdToIndexGraph;
	}

	return *this;
}

bool Instance::isExist(DFSCode& dfsCode)
{
	for (unordered_set<DFSCode>::iterator it = childIDs.begin(); it != childIDs.end(); ++it)
	{
		if (*it == dfsCode)
			return true;
	}

	return false;
}

void Instance::push(Graph& g, unordered_set<DFSCode> child)
{
	graphs.push_back(g);
	mapIdToIndexGraph[g.idGraph] = graphs.size() - 1;

	childIDs.insert(child.begin(), child.end());
}

void Instance::push(Graph& g, unordered_set<DFSCode> child, unordered_set<CodeId> ignore)
{
	graphs.push_back(g);
	mapIdToIndexGraph[g.idGraph] = graphs.size() - 1;

	childIDs.insert(child.begin(), child.end());
	ignoreList.insert(ignore.begin(), ignore.end());
}

void Instance::push(Graph& g)
{
	graphs.push_back(g);
	mapIdToIndexGraph[g.idGraph] = graphs.size() - 1;
}

void Instance::assign(vector<Graph>& listGraph)
{
	graphs.clear();
	graphs = listGraph;
	mapIdToIndexGraph.clear();
	for (int i = 0; i < (int) graphs.size(); i++)
	{
		mapIdToIndexGraph[graphs[i].idGraph] = i;
	}
}

void Instance::assign(vector<Graph>& listGraph, int freq_)
{
	graphs.clear();
	graphs = listGraph;
	mapIdToIndexGraph.clear();
	for (int i = 0; i < (int) graphs.size(); i++)
	{
		mapIdToIndexGraph[graphs[i].idGraph] = i;
	}
	this->freq = freq_;
}

void Instance::assign(vector<Graph>& listGraph, unordered_set<CodeId> ignore)
{
	graphs.clear();
	graphs = listGraph;
	mapIdToIndexGraph.clear();
	for (int i = 0; i < (int) graphs.size(); i++)
	{
		mapIdToIndexGraph[graphs[i].idGraph] = i;
	}
	ignoreList.insert(ignore.begin(), ignore.end());
}

void Instance::insertIgnoreList(unordered_set<CodeId>& ignore)
{
	ignoreList.insert(ignore.begin(), ignore.end());
}

int Instance::computeFrequency()
{
	if (this->freq <= 0)
	{
		int size = graphs.size();
		OverlapGraph ovgraph(size);
	
		for (int i = 0; i < size; i++)
		{
			ovgraph.add(&graphs[i], i);
		}

		//freq = ovgraph.getMISSize(independent_graphs);
		freq = ovgraph.getMISSize();
	}

	return freq;
}