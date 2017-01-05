#ifndef TOPKQUEUE_H
#define TOPKQUEUE_H

#include "correlatedresult.h"
#include <queue>
#include <vector>
#include <fstream>

using namespace std;

class sortNodesByPrio
{
public:
	bool operator () (const CorrelatedResult &n1, const CorrelatedResult  &n2)   const
	{
		if (n1.colocatedvalue > n2.colocatedvalue)
			return true;
		else if (n1.colocatedvalue == n2.colocatedvalue)
		{
			return n1.confidencevalue > n2.confidencevalue;
		}
		else
			return false;
	}
};

class TopKQueue
{
public:
	TopKQueue(unsigned int k) : _k(k) {}
	void insert(CorrelatedResult & result);
	void print(ofstream & of);
	vector<CorrelatedResult> reverse();
	bool isFull();
	int minCorrelatedValue();

private:
	unsigned int _k;
	priority_queue<CorrelatedResult, vector<CorrelatedResult>, sortNodesByPrio> _queue;
};

#endif