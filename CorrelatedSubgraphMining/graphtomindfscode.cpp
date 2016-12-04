#include "graphtomindfscode.h"

void GraphToMinDFSCode::init ()
{
    m_IsFound = false;

    TRANS.clear();
    DFS_CODE.clear();
    DFS_CODE_IS_MIN.clear();
    GRAPH_IS_MIN.clear();
    m_MinDFSCode.clear();
}

void GraphToMinDFSCode::findMinimumDFSCode (Graph* graph, DFSCode& dfs_code)
{
	init();

	Graph super_graph = *graph;
    TRANS.push_back(super_graph);
	graph_size = graph->edge_size();

    Projected_map3 root;
    EdgeList edges;

	if (super_graph.size() == 1)
	{
		dfs_code.push(-1, 0, -1, -1, super_graph[0].label);
		return;
	}

    for (unsigned int from = 0; from < super_graph.size(); ++from)
	{
      if (get_forward_root(super_graph, super_graph[from], edges))
	  {
        for (EdgeList::iterator it = edges.begin(); it != edges.end(); ++it)
		{
			root[super_graph[from].label][(*it)->elabel][super_graph[super_graph.index((*it)->to)].label].push(0, *it, 0);
        }
      }
    }

    for (Projected_iterator3 fromlabel = root.begin(); fromlabel != root.end(); fromlabel++) 
	{
      for (Projected_iterator2 elabel = fromlabel->second.begin(); elabel != fromlabel->second.end(); elabel++)
	  {
        for (Projected_iterator1 tolabel = elabel->second.begin(); tolabel != elabel->second.end(); tolabel++)
		{
          DFS_CODE.push(0, 1, fromlabel->first, elabel->first, tolabel->first);
          _Start(tolabel->second);
          DFS_CODE.pop();
        }
      }
    }
	
    dfs_code = m_MinDFSCode;
}

void GraphToMinDFSCode::_Start(Projected& projected)
{
	if (m_IsFound)
      return;

    if (!is_min())
      return;

    if (DFS_CODE.size() == graph_size) {
      m_IsFound = true;
      m_MinDFSCode = DFS_CODE;
      return;
    }

    const RMPath &rmpath = DFS_CODE.buildRMPath();
    int minlabel = DFS_CODE[0].fromlabel;
    int maxtoc = DFS_CODE[rmpath[0]].to;

    Projected_map3 new_fwd_root;
    Projected_map2 new_bck_root;
    EdgeList edges;

    for (unsigned int n = 0; n < projected.size(); ++n)
	{
      unsigned int id = projected[n].id;
      PDFS *cur = &projected[n];
      History history(TRANS[id], cur);

      for (int i = (int) rmpath.size() - 1; i >= 1; --i)
	  {
        Edge *e = get_backward(TRANS[id], history[rmpath[i]], history[rmpath[0]], history);
        if (e)
          new_bck_root[DFS_CODE[rmpath[i]].from][e->elabel].push(id, e, cur);
      }

      if (get_forward_pure(TRANS[id], history[rmpath[0]], minlabel, history, edges))
	  {
        for (EdgeList::iterator it = edges.begin(); it != edges.end(); ++it)
		{
			new_fwd_root[maxtoc][(*it)->elabel][TRANS[id][TRANS[id].index((*it)->to)].label].push(id, *it, cur);
        }
      }

      for (int i = 0; i < (int) rmpath.size(); ++i)
	  {
        if (get_forward_rmpath(TRANS[id], history[rmpath[i]], minlabel, history, edges))
		{
          for (EdgeList::iterator it = edges.begin(); it != edges.end(); ++it)
		  {
			  new_fwd_root[DFS_CODE[rmpath[i]].from][(*it)->elabel][TRANS[id][TRANS[id].index((*it)->to)].label].push(id, *it, cur);
          }
        }
      }
    }

    for (Projected_iterator2 to = new_bck_root.begin(); to != new_bck_root.end(); ++to)
	{
      for (Projected_iterator1 elabel = to->second.begin(); elabel != to->second.end(); ++elabel)
	  {
         DFS_CODE.push(maxtoc, to->first, -1, elabel->first, -1);
         _Start(elabel->second);
         DFS_CODE.pop();
      }
    }

    for (Projected_riterator3 from = new_fwd_root.rbegin(); from != new_fwd_root.rend(); ++from)
	{
      for (Projected_iterator2 elabel = from->second.begin(); elabel != from->second.end(); ++elabel)
	  {
        for (Projected_iterator1 tolabel = elabel->second.begin(); tolabel != elabel->second.end(); ++tolabel)
		{
			DFS_CODE.push(from->first, maxtoc + 1, -1, elabel->first, tolabel->first);
			_Start(tolabel->second);
			DFS_CODE.pop();
        }
      }
    }
}

bool GraphToMinDFSCode::is_min()
{
	if (DFS_CODE.size() == 1)
      return (true);

    DFS_CODE.toGraph(GRAPH_IS_MIN);
    DFS_CODE_IS_MIN.clear();

    Projected_map3 root;
    EdgeList edges;

    for (unsigned int from = 0; from < GRAPH_IS_MIN.size(); ++from)
      if (get_forward_root(GRAPH_IS_MIN, GRAPH_IS_MIN[from], edges))
        for (EdgeList::iterator it = edges.begin(); it != edges.end(); ++it)
			root[GRAPH_IS_MIN[from].label][(*it)->elabel][GRAPH_IS_MIN[GRAPH_IS_MIN.index((*it)->to)].label].push(0, *it, 0);

    Projected_iterator3 fromlabel = root.begin();
    Projected_iterator2 elabel = fromlabel->second.begin();
    Projected_iterator1 tolabel = elabel->second.begin();

    DFS_CODE_IS_MIN.push(0, 1, fromlabel->first, elabel->first, tolabel->first);

    return (project_is_min(tolabel->second));
}

bool GraphToMinDFSCode::project_is_min(Projected& projected)
{
    const RMPath& rmpath = DFS_CODE_IS_MIN.buildRMPath();
    int minlabel = DFS_CODE_IS_MIN[0].fromlabel;
    int maxtoc = DFS_CODE_IS_MIN[rmpath[0]].to;

    {
      Projected_map1 root;
      bool flg = false;
      int newto = 0;

      for (int i = rmpath.size() - 1; !flg && i >= 1; --i) 
	  {
		for (unsigned int n = 0; n < projected.size(); ++n) 
		{
			PDFS *cur = &projected[n];
			History history(GRAPH_IS_MIN, cur);
            Edge *e = get_backward(GRAPH_IS_MIN, history[rmpath[i]], history[rmpath[0]], history);

            if (e) 
			{
				root[e->elabel].push(0, e, cur);
				newto = DFS_CODE_IS_MIN[rmpath[i]].from;
				flg = true;
            }
        }
      }

      if (flg) 
	  {
        Projected_iterator1 elabel = root.begin();
        DFS_CODE_IS_MIN.push(maxtoc, newto, -1, elabel->first, -1);
        
		if (DFS_CODE[DFS_CODE_IS_MIN.size() - 1] != DFS_CODE_IS_MIN[DFS_CODE_IS_MIN.size() - 1])
			return false;
        
		return project_is_min(elabel->second);
      }
    }

    {
      bool flg = false;
      int newfrom = 0;
      Projected_map2 root;
      EdgeList edges;

      for (unsigned int n = 0; n < projected.size(); ++n)
	  {
		PDFS *cur = &projected[n];
        History history(GRAPH_IS_MIN, cur);
        if (get_forward_pure(GRAPH_IS_MIN, history[rmpath[0]], minlabel, history, edges))
		{
          flg = true;
          newfrom = maxtoc;
          for (EdgeList::iterator it = edges.begin(); it != edges.end(); ++it)
			  root[(*it)->elabel][GRAPH_IS_MIN[GRAPH_IS_MIN.index((*it)->to)].label].push(0, *it, cur);
        }
      }

      for (int i = 0; !flg && i < (int) rmpath.size(); ++i)
	  {
        for (unsigned int n = 0; n < projected.size(); ++n)
		{
          PDFS *cur = &projected[n];
          History history(GRAPH_IS_MIN, cur);

          if (get_forward_rmpath(GRAPH_IS_MIN, history[rmpath[i]], minlabel, history, edges))
		  {
            flg = true;
            newfrom = DFS_CODE_IS_MIN[rmpath[i]].from;

            for (EdgeList::iterator it = edges.begin();it != edges.end(); ++it)
				root[(*it)->elabel][GRAPH_IS_MIN[GRAPH_IS_MIN.index((*it)->to)].label].push(0, *it, cur);
          }
        }
      }

      if (flg)
	  {
        Projected_iterator2 elabel = root.begin();
        Projected_iterator1 tolabel = elabel->second.begin();

        DFS_CODE_IS_MIN.push(newfrom, maxtoc + 1, -1, elabel->first, tolabel->first);
        if (DFS_CODE[DFS_CODE_IS_MIN.size() - 1] != DFS_CODE_IS_MIN[DFS_CODE_IS_MIN.size() - 1])
          return false;

        return project_is_min(tolabel->second);
      }
    }

    return true;
 }