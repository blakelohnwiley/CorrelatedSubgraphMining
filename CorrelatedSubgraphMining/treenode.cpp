#include "treenode.h"

DFSCode TreeNode::computeDFSCode()
{
	GraphToMinDFSCode convertGraphToDFSCode;
	convertGraphToDFSCode.findMinimumDFSCode(&graph, code);
	return code;
}

bool TreeNode::isDuplicated(Graph& g)
{
	return graph.isDuplicated(g);
}