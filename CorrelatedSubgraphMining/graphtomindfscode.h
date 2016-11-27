#ifndef GRAPHTOMINDFSCODE_H
#define GRAPHTOMINDFSCODE_H

#include "graphcode.h"

class GraphToMinDFSCode
{
protected:
	Graph* m_Graph;
	DFSCode m_MinDFSCode;
	bool m_IsFound;

	vector<Graph> TRANS;
	DFSCode DFS_CODE;
	DFSCode DFS_CODE_IS_MIN;
	Graph GRAPH_IS_MIN;
	//store the number of edges of a graph
	int graph_size;

	void _Start(Projected& projected);
	bool is_min();
	bool project_is_min(Projected& projected);

public:
	void init(Graph* graph);
	void findMinimumDFSCode(DFSCode& dfs_code);
};

#endif