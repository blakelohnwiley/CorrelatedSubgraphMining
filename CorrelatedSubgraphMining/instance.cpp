#include "instance.h"

bool Instance::isExist(DFSCode& dfsCode)
{
	for (set<DFSCode>::iterator it = childIDs.begin(); it != childIDs.end(); ++it)
	{
		if (*it == dfsCode)
			return true;
	}

	return false;
}

void Instance::push(Graph& g, set<DFSCode> child)
{
	graphs.push_back(g);

	childIDs.insert(child.begin(), child.end());
}

void Instance::push(Graph& g, set<DFSCode> child, set<CodeId> ignore)
{
	graphs.push_back(g);

	childIDs.insert(child.begin(), child.end());
	ignoreList.insert(ignore.begin(), ignore.end());
}

void Instance::push(Graph& g)
{
	graphs.push_back(g);
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

		freq = ovgraph.getMISSize(independent_graphs);
	}

	return freq;
}