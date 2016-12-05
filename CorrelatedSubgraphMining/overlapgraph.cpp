#include "overlapgraph.h"

OGNode::OGNode(Graph* emb, int index)
{
	this->emb = emb;
	this->deg = 0;
	this->index = index;
	this->mark = -1;
}

/* --- mark connected component */
void OGNode::markConnectComp (int mark)
{
	this->mark = mark; /* mark the node as visited */
	/* traverse the adjacent nodes recursively mark unvisited nodes */
	for (int i = 0; i < this->deg; i++)
	{
		if (this->adjs[i]->mark < 0)
			this->adjs[i]->markConnectComp(mark);
    }
}

/* --- compare to another node */
bool OGNode::operator< (OGNode& obj)
{
	if (this->mark < obj.mark) 
		return true;

	return false;
}

bool OGNode::operator== (OGNode& obj)
{
	if (this->mark == obj.mark) 
		return true;

	return false;
}

bool OGNode::operator> (OGNode& obj)
{
	if (this->mark > obj.mark) 
		return true;

	return false;
}

void OverlapGraph::add (Graph* emb, int index)
{
	OGNode* d;

	for (int i = 0; i < this->cnt; i++)
	{
		d = this->nodes[i];        /* traverse the existing nodes */
		d->mark = (emb->overlap(*(d->emb)) == true) ? 1 : 0;
    }  

	/* create a new node and add it to the node array*/
	OGNode* s = new OGNode(emb, index);
	this->nodes[this->cnt] = s;
	this->cnt++;
	
	/* traverse the existing embeddings, but skip non-overlapping ones. Add an edge between the nodes*/
	for (int i = 0; i < this->cnt - 1; i++)
	{
		d = this->nodes[i];
		if (d->mark <= 0)
			continue;

		d->deg++;
		d->adjs.push_back(this->nodes[this->cnt - 1]);
		this->nodes[this->cnt - 1]->deg++;
		nodes[this->cnt - 1]->adjs.push_back(d);
	}
}

void OverlapGraph::sort(OGNode** buffer, int numelement)
{
	for (int i = 0; i < numelement - 1; i++)
	{
		OGNode minEle = *buffer[i];
		int min = i;

		for (int j = i + 1; j < numelement; j++)
		{
			if (minEle < *buffer[j])
			{
				min = j;
				minEle = *buffer[j];
			}
		}

		if (min != i)
		{
			OGNode* tmp = buffer[i];
			buffer[i] = buffer[min];
			buffer[min] = tmp;
		}
	}
}

int OverlapGraph::getMISSize (vector<int>& selectedGraph)
{
	int    i, k, n;             /* loop variables, number of nodes */
    int    c;                   /* number of connected components */
    OGNode* s;                   /* to traverse the nodes */

    /* --- handle trivial cases --- */
    if (this->cnt <= 1) 
	{
		this->selectedGraph.push_back(this->nodes[0]->index);
		selectedGraph = this->selectedGraph;
		return this->cnt;
	}

    if (this->cnt <= 2)
	{
		if (this->nodes[0]->deg > 0)
		{
			this->selectedGraph.push_back(this->nodes[0]->index);
		}
		else
		{
			this->selectedGraph.push_back(this->nodes[0]->index);
			this->selectedGraph.push_back(this->nodes[1]->index);
		}
		selectedGraph = this->selectedGraph;
		return (2 - this->nodes[0]->deg);
	}

    /* --- select all safe nodes --- */
    this->sel = n = 0;           /* initialize the node counters */
    for (i = this->cnt; --i >= 0; )
	{
		s = this->nodes[i];     /* traverse the nodes and */
		s->red = s->deg;            /* copy the node degree */
		if (s->deg <= 0)
		{ 
			s->mark =  0; 
			this->selectedGraph.push_back(s->index);
			this->sel++;
		}
		else if (s->deg <= 1)
		{
			s->mark = -1;
			n++;
		}
		else s->mark = -1;        /* select the isolated nodes and */
    }                             /* mark other nodes as unprocessed */

    this->rem = this->cnt - this->sel;

    if (n > 0)
	{                /* if there are leaf nodes */
		for (i = this->cnt; --i >= 0; )
		{
			s = this->nodes[i];      /* traverse the nodes again */
			if ((s->mark < 0) && (s->red == 1)) this->selectSafe(s);
		}                       /* select all leaf nodes */
    }                           /* (and other safe nodes recursively) */

	/* If there are at most three nodes left, there are either zero */
    /* or three nodes left, because after selecting all safe nodes, */
    /* all remaining nodes must have at least two incident edges.   */
    /* With three nodes, the remaining graph must be a triangle.    */
    if (this->rem <= 3) 
	{
		if (this->rem > 0)
		{
			for (i = this->cnt; --i > 0; )
			{
				if (this->nodes[i]->mark < 0)
				{
					this->selectedGraph.push_back(this->nodes[i]->index);
					break;
				}
			}
		}
		selectedGraph = this->selectedGraph;
		return this->sel += this->rem & 1;
	}

    /* --- split into connected components --- */
    this->buf = new OGNode* [this->rem];
	int savebufsize = this->rem;

    for (i = c = 0; i < this->cnt; i++)
	{
		s = this->nodes[i];        /* traverse the nodes */
		if (s->mark < 0)
			this->buf[c++] = s;
    }                           /* copy remaining nodes to the buffer */

    for (i = c = 0; i < this->rem; i++)
	{
		s = this->buf[i];          /* traverse the nodes again */
		if (s->mark < 0)
			s->markConnectComp(c++);
    }                           /* mark the connected components */

    sort(buf, savebufsize);      /* sort nodes according to markers */

    /* --- process the connected components --- */
    i = this->rem;               /* create a stack for exact algorithm */

    while (--c >= 0)
	{          /* traverse the connected components */
		for (k = i; --i >= 0; )   /* find the start of the component */
			if (this->buf[i]->mark != c) break;

		this->rem = n = k - (++i);  /* compute the number of nodes */
		/* The number of nodes in each connected component must be at */
		/* least three, and each node must have at least two incident */
		/* edges, because otherwise the nodes would have been fixed   */
		/* when safe nodes where selected (see above).                */
		if (n <= 3)
		{ 
			/* If the connected component contains exactly three nodes,   */
			/* it must be triangle, so exactly one node can be selected.  */			
			this->selectedGraph.push_back(buf[k - 1]->index);	
			this->sel++;
			continue;
		}

		for (n = k; --n >= i; )   /* traverse the nodes of the comp. */
			this->buf[n]->mark = -1;  /* and clear the node markers */
		greedy(i, k);
    }  

    delete [] buf;          /* and the node buffer */

	selectedGraph = this->selectedGraph;
    return this->sel;            /* return the size of an MIS */
}

void OverlapGraph::greedy (int beg, int end)
{                             /* --- process a component greedily */
    int    i, k, m = 1;         /* loop variables */
    OGNode *node, *s, *d;          /* to traverse the nodes */

    do
	{                        /* greedy node selection loop */
		while (this->buf[--end]->mark >= 0);    /* find the next unprocessed node */

		for (node = this->buf[i = k = end]; --i >= beg; )
		{
			d = this->buf[i];        /* traverse the remaining nodes */
			if ((d->mark < 0) && (d->red < node->red))
			{
				node = d;
				k = i;
			}
		}                         /* find a node with minimum degree */

		buf[k]   = buf[end];
		buf[end] = node;     /* swap the found node to the end */
		node->mark = 0;            /* mark and count it as selected and */
		selectedGraph.push_back(node->index);
		this->sel++; m++;          /* remove the node and its neighbors */

		this->rem -= node->red +1;  /* from the remaining nodes */
		for (i = node->deg; --i >= 0;)
		{
			s = node->adjs[i];
		
			if (s->mark < 0)
				s->mark = m; 
		}

		for (i = node->deg; --i >= 0; )
		{
			s = node->adjs[i];       /* traverse the excluded neighbors */
			if (s->mark != m)
				continue;

			s->red = 0;              /* initialize the safe node counter */
			for (k = s->deg; --k >= 0; )
			{
				d = s->adjs[k];        /* traverse each neighbor's neighbors */
				if ((d->mark < 0) && (--d->red <= 1))
					s->red++;
			}                       /* reduce their node degrees and */
		}                         /* count leaves and isolated nodes */

		for (i = node->deg; --i >= 0; )
		{
			s = node->adjs[i];       /* traverse the node's neighbors */
			if ((s->mark != m) || (s->red <= 0)) continue;
			for (k = s->deg; --k >= 0; )
			{
				d = s->adjs[k];        /* traverse each neighbor's neighbors */
				if ((d->mark < 0) && (d->red <= 1))
					selectSafe(d);
			}                       /* select unprocessed leaves and */
		}                         /* isolated nodes recursively */
    }
	while (this->rem > 3);     /* while the rest is not trivial */
    
	if (this->rem > 0)
	{
		for (i = beg; i <= end; i++)
		{
			if (this->buf[i]->mark < 0)
			{
				this->selectedGraph.push_back(this->buf[i]->index);
				break;
			}
		}
	}

	this->sel += this->rem & 1;   /* process the trivial rest */
 }

void OverlapGraph::selectSafe (OGNode* node)
{                               /* --- select a safe node */
    int    i, n;                /* loop variables */
    OGNode *s, *d;                /* to traverse the nodes */

    node->mark = 0;              /* mark and count node as selected */
	this->selectedGraph.push_back(node->index);
    this->sel++; this->rem--;     /* and remove it from the rem. nodes */
    
	if (node->red <= 0) 
		return;  /* if the node is isolated, abort */

	for (i = node->deg; node->adjs[--i]->mark >= 0; );

	s = node->adjs[i];           /* find the only neighbor of a leaf, */
    s->mark = 1;                 /* mark it as excluded, and remove */
    this->rem--; n = 0;          /* it from the remaining nodes */

    for (i = s->deg; --i >= 0; )
	{
		d = s->adjs[i];            /* reduce its neighbors degrees */
		if ((d->mark < 0) && (--d->red <= 1)) n++;
    }                           /* count leaves and isolated nodes */

    if (n <= 0)
		return;         /* if no recursion is needed, abort */

    for (i = s->deg; --i >= 0; )
	{
		d = s->adjs[i];            /* traverse the neighbor's neighbors */
		if ((d->mark < 0) && (d->red <= 1))
			this->selectSafe(d);
    }                           /* select safe nodes recursively */
}