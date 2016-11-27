/*
 *  
 *
 */

#include "graphcode.h"

using namespace std;

const RMPath& DFSCode::buildRMPath()
{
	rmpath.clear();
	int old_from = -1;

	for (int i = size() - 1; i >= 0; --i)
	{
		if ((*this)[i].from < (*this)[i].to && (rmpath.empty() || old_from == (*this)[i].to))
		{
			rmpath.push_back(i);
			old_from = (*this)[i].from;
		}
	}
	
	return rmpath;
}

void History::build(Graph& g, PDFS* e)
{
	clear();
	edge.clear();
	edge.resize(g.edge_size());
	vertex.clear();
	vertex.resize(g.size());
	
	if (e)
	{
		push_back(e->edge);
		edge[e->edge->id] = vertex[g.index(e->edge->from)] = vertex[g.index(e->edge->to)] = 1;
		
		for (PDFS* p = e->prev; p; p = p->prev)
		{
			push_back(p->edge);
			edge[p->edge->id] = vertex[g.index(p->edge->from)] = vertex[g.index(p->edge->to)] = 1;
		}
		
		reverse(begin(), end());
	}
}

bool get_forward_rmpath(Graph &graph, Edge *e, int minlabel, History& history, EdgeList &result)
{
	result.clear ();
	
	int tolabel = graph[graph.index(e->to)].label;
	int indexFrom = graph.index(e->from);

	for (Vertex::edge_iterator it = graph[indexFrom].edge.begin(); it != graph[indexFrom].edge.end(); ++it)
	{
		int tolabel2 = graph[graph.index(it->to)].label;
		if (e->to == it->to || minlabel > tolabel2 || history.hasVertex (graph.index(it->to)))
			continue;
		
		if (e->elabel < it->elabel || (e->elabel == it->elabel && tolabel <= tolabel2))
			result.push_back (&(*it));
	}
	
	return (!result.empty());
}

bool get_forward_pure (Graph &graph, Edge *e, int minlabel, History& history, EdgeList &result)
{
	result.clear ();
	int indexTo = graph.index(e->to);

	for (Vertex::edge_iterator it = graph[indexTo].edge.begin(); it != graph[indexTo].edge.end(); ++it)
	{
		if (minlabel > graph[graph.index(it->to)].label || history.hasVertex(graph.index(it->to)))
			continue;
		
		result.push_back (&(*it));
	}
	
	return (!result.empty());
}

bool get_forward_root (Graph &g, Vertex &v, EdgeList &result)
{
	result.clear ();

	for (Vertex::edge_iterator it = v.edge.begin(); it != v.edge.end(); ++it)
	{
		if (v.label <= g[g.index(it->to)].label)
			result.push_back (&(*it));
	}
	
	return (!result.empty());
}

Edge *get_backward (Graph &graph, Edge* e1, Edge* e2, History& history)
{
	if (e1 == e2)
		return 0;
	
	int indexe2To = graph.index(e2->to);
	int indexe1To = graph.index(e1->to);

	for (Vertex::edge_iterator it = graph[indexe2To].edge.begin(); it != graph[indexe2To].edge.end(); ++it)
	{
		if (history.hasEdge(it->id))
			continue;
		
		if ( (it->to == e1->from) &&
			( (e1->elabel < it->elabel) ||
			 (e1->elabel == it->elabel) &&
			 (graph[indexe1To].label <= graph[indexe2To].label)
			 ) )
		{
			return &(*it);
		}
	}
	
	return 0;
}
