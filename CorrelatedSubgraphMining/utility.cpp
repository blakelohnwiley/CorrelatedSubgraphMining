#include "utility.h"

bool Utility::isExistEdgeInList(vector<Edge> & edgeList, const Edge & e)
{
	for (vector<Edge>::iterator it = edgeList.begin(); it < edgeList.end(); ++it)
	{
		if ((*it) == e)
		{
			return true;
		}
	}

	return false;
}