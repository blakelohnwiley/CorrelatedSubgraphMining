#include "treenode.h"

string TreeNode::computeDFSCode()
{
	GraphToMinDFSCode convertGraphToDFSCode;
	DFSCode dfscode;
	convertGraphToDFSCode.findMinimumDFSCode(&graph, dfscode);
	code = dfscode.toString();
	return code;
}

bool TreeNode::isDuplicated(Graph& g)
{
	return graph.isDuplicated(g);
}