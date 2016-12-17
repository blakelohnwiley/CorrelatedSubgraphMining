/**
 * This class to create an overlap graph in order to find maximum independent set
*/

#ifndef OVERLAPGRAPH_H
#define OVERLAPGRAPH_H
#include "graph.h"
#include <vector>

using namespace std;

class OGNode
{
public:
	Graph* emb;
	/** the node degree (number of incident edges) */
	int deg;
	/** the reduced node degree (remaining graph without fixed nodes) */
	int	red;
	/** the node marker (for connected components/recursion depth) */
	int	mark;
	/** the array of adjacent nodes */
	vector<OGNode*> adjs;

	/*index of graph in list of instances*/
	int index;

	OGNode(Graph* emb, int index_graph);
	OGNode() {}
   /*------------------------------------------------------------------*/
   /** Recursively mark the nodes of a connected component.
   *  @param  mark the value with which to mark the nodes */
   /*------------------------------------------------------------------*/

	void markConnectComp (int mark);

  /*------------------------------------------------------------------*/
  /** Compare to another node.
   *  @param  obj the node to compare to */
  /*------------------------------------------------------------------*/

	bool operator< (OGNode& obj);
	bool operator== (OGNode& obj);
	bool operator> (OGNode& obj);
};

class OverlapGraph 
{
private:
	/** the (current) number of nodes */
	int	cnt;
	/** the array of nodes */
	OGNode** nodes;
	/** the number of selected nodes */
	int	sel;
	/** the number of remaining nodes (neither selected nor excluded) */
	int	rem;
	/** the number of nodes in the best independent set found yet */
	int	best;
	/** the buffer for the nodes (for reordering) */
	OGNode** buf;
	/** the next position on the stack */
	int	pos;

	void sort(OGNode** buffer, int numelement);

public:

	//save index of selected nodes into MIS
	//vector<int> selectedGraph;

	OverlapGraph (int size) { nodes = new OGNode* [size]; cnt = 0; buf = NULL; }
	int size () { return this->cnt; }
	
	void clear() { 
		for (int i = this->cnt; --i >= 0;)
			delete nodes[i];
		delete [] nodes;
		cnt = 0;
	}

	~OverlapGraph()
	{
		this->clear();
	}
   /*------------------------------------------------------------------*/
   /** Add an embedding to the overlap graph.
   *  <p>The embedding is compared to all previously added embeddings
   *  and it is checked whether there is a overlap with them.
   *  If there is an overlap, the nodes representing the embeddings are
   *  connected with an edge.</p>
   *  @param  emb the embedding to add */
  /*------------------------------------------------------------------*/

  void add(Graph* emb, int index);

  /*------------------------------------------------------------------*/
  /** Find the size of a maximum independent node set (MIS).
   *  <p>The search is terminated as soon as an independent set of
   *  at least the minimum required size is found. In this case the
   *  returned value may be smaller than the actual MIS size, even
   *  if the exact algorithm is used.</p>
   *  @param  selectedGraph: instances of graph is selected into an independent set
   *  @return the size of a maximum independent node set
   *  @since  2007.06.14 (Christian Borgelt) */
  /*------------------------------------------------------------------*/
  //int getMISSize(vector<int> & selectedGraph);
  int getMISSize();

  /*------------------------------------------------------------------*/
  /** Select a safe node for an independent node set.
   *  <p>Isolated nodes and leaves are safe to select. For isolated
   *  nodes this is trivial. For leaf nodes there must be a maximum
   *  independent set containing them, because selecting them is less
   *  restrictive that selecting their only adjacent node. On the other
   *  hand, if the only adjacent node is not selected, the leaf can be
   *  selected for an independent set.</p>
   *  <p>Note that, in general, any node the neighbors of which form
   *  a complete graph is safe to select. However, this test is more
   *  complicated and thus has not been implemented up to now.</p>
   *  <p>The given node, which must be an isolated node or a leaf,
   *  is selected and its neighbors are excluded. In addition, any
   *  neighbors of these excluded neighbors that become safe to select
   *  due to the exclusion (that is, which are isolated nodes or leaves
   *  in the reduced overlap graph) are selected (by calling this
   *  function recursively).</p>
   *  @param  node the node to select
  /*------------------------------------------------------------------*/

  void selectSafe(OGNode* node);

  /*------------------------------------------------------------------*/
  /** Process a connected component greedily.
   *  @param  beg the start of the node range (included)
   *  @param  end the end   of the node range (excluded)
  /*------------------------------------------------------------------*/

  void greedy(int beg, int end);
};

#endif