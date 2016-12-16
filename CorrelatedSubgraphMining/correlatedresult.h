#ifndef CORRELATEDRESULT_H
#define CORRELATEDRESULT_H
#include "graph.h"
#include <fstream>
using namespace std;

class CorrelatedResult
{
public:
	Graph g1, g2;
	int colocatedvalue;
	double confidencevalue;

	bool operator > (const CorrelatedResult & d2) const
	{
		return colocatedvalue > d2.colocatedvalue;
	}

	bool operator < (const CorrelatedResult & d2) const
	{
		return colocatedvalue < d2.colocatedvalue;
	}

	bool operator == (const CorrelatedResult & d2) const
	{
		return colocatedvalue == d2.colocatedvalue;
	}

	void report_result(ofstream & of, int id);
};


#endif