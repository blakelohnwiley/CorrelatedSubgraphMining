#include "TopKQueue.h"

void TopKQueue::insert(CorrelatedResult & result)
{
	if (this->_queue.size() < _k)
	{
		_queue.push(result);
	}
	else if (result.colocatedvalue > _queue.top().colocatedvalue || (result.colocatedvalue == _queue.top().colocatedvalue && result.confidencevalue > _queue.top().confidencevalue))
	{
		_queue.pop();
		_queue.push(result);
	}
}

void TopKQueue::print(ofstream & of)
{
	vector<CorrelatedResult> result = reverse();
	for (unsigned int i = 0; i < result.size(); i++)
	{
		result[i].report_result(of, i + 1);
	}
}

vector<CorrelatedResult> TopKQueue::reverse()
{
	priority_queue<CorrelatedResult, vector<CorrelatedResult>, sortNodesByPrio> tmpQ = _queue;
	vector<CorrelatedResult> result(tmpQ.size());
	while (!tmpQ.empty())
	{
		result[tmpQ.size() - 1] = tmpQ.top();
		tmpQ.pop();
    }
      return result;
}

bool TopKQueue::isFull()
{
	return _queue.size() >= _k;
}

int TopKQueue::minCorrelatedValue()
{
	return _queue.top().colocatedvalue;
}