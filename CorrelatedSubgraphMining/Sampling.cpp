#include "Sampling.h"
#include <algorithm>

uint64_t Sampling::id = 0;
Hashtable Sampling::saveGraph;
set<DFSCode> Sampling::noneFrequentGraph;

Sampling::Sampling(char * fileInput, bool directed_)
{
	this->directed = directed_;
	database.directed = directed;
	database.read(fileInput);
}

int Sampling::computeFrequency(vector<Graph>& listGraph)
{
	int size = (int) listGraph.size();
	OverlapGraph ovgraph(size);
	
	for (int i = 0; i < size; i++)
	{
		ovgraph.add(&listGraph[i], i);
	}

	return ovgraph.getMISSize();
}

//Sample Sampling::uniformSamplingExactGraph(int threshold, int miniter, int maxIter)
//{
//	int pFreq = 0;
//	Graph p = database.initAnyFrequentGraph(threshold, pFreq);
//	vector<Graph> upNeighbors;
//	vector<Graph> downNeighbors;
//	upNeighbors = p.getUpNeigborsExactGraph(database);
//	downNeighbors = p.getDownNeighborsExactGraph();
//
//	vector<Graph> neighbors;
//	for (vector<Graph>::iterator gr = upNeighbors.begin(); gr != upNeighbors.end(); ++gr)
//	{
//		vector<Graph> insts = dualISO.match(database, *gr);
//		if (computeFrequency(insts) >= threshold)
//		{
//			neighbors.push_back(*gr);
//		}
//	}
//
//	for (vector<Graph>::iterator gr = downNeighbors.begin(); gr != downNeighbors.end(); ++gr)
//	{
//		vector<Graph> insts = dualISO.match(database, *gr);
//		if (computeFrequency(insts) >= threshold)
//		{
//			neighbors.push_back(*gr);
//		}
//	}
//
//	int dp = (int) neighbors.size();
//
//	if (dp == 0)
//	{
//		Sample s;
//		s.status = false;
//		return s;
//	}
//
//	int iter = 0;
//	int g = 0;
//	int previousSelect = -1;
//	Graph q(database.directed);
//	vector<Graph> tmpNeighbors;
//
//	while (true)
//	{
//		double rnd = rand() / (1.0 * RAND_MAX);
//		int indxSelected = (int) (rnd * (dp - 1) + 0.5);
//
//		if (indxSelected != previousSelect)
//		{
//			q.clear();
//			q = neighbors[indxSelected];
//
//			previousSelect = indxSelected;
//			tmpNeighbors.clear();
//			
//			upNeighbors.clear();
//			downNeighbors.clear();
//			//cout << "Getting Up" << endl;
//			upNeighbors = q.getUpNeigborsExactGraph(database);
//			//cout << "Getting Down" << endl;
//			downNeighbors = q.getDownNeighborsExactGraph();
//
//			for (vector<Graph>::iterator gr = upNeighbors.begin(); gr != upNeighbors.end(); ++gr)
//			{
//				//cout << "Matching" << endl;
//				vector<Graph> insts = dualISO.match(database, *gr);
//				//cout << "Freq up" << endl;
//				if (computeFrequency(insts) >= threshold)
//				{
//					tmpNeighbors.push_back(*gr);
//				}
//			}
//
//			for (vector<Graph>::iterator gr = downNeighbors.begin(); gr != downNeighbors.end(); ++gr)
//			{
//				//cout << "Matching" << endl;
//				vector<Graph> insts = dualISO.match(database, *gr);
//				//cout << "Freq down" << endl;
//				if (computeFrequency(insts) >= threshold)
//				{
//					tmpNeighbors.push_back(*gr);
//				}
//			}
//		}
//
//		int dq = (int) tmpNeighbors.size();
//		double accept_prob = 1;
//		double tmp = (dp * 1.0 / dq);
//		if (tmp < accept_prob)
//		{
//			accept_prob = tmp;
//		}
//
//		rnd = rand() / (1.0 * RAND_MAX);
//		//cout << "Accepted Rate: " << accept_prob << endl;
//		if (rnd <= accept_prob || g > maxIter)
//		{
//			//cout << "One sample is selected" << endl;
//			p.clear();
//			p = q;
//			previousSelect = -1;
//			++iter;
//			g = 0;
//
//			if (iter >= miniter)
//			{
//				Sample s;
//				s.p = p;
//				s.isomophisms = dualISO.match(database, p);
//				s.freq = computeFrequency(s.isomophisms);
//				for (int tt = 0; tt < s.isomophisms.size(); tt++)
//				{
//					s.isomophisms[tt].idGraph = id;
//					++id;
//				}
//				s.status = true;
//
//				return s;
//			}
//			else
//			{
//				//cout << "Next" << endl;
//				neighbors.clear();
//				neighbors = tmpNeighbors;
//				dp = (int) tmpNeighbors.size();
//			}
//		}
//		else
//		{
//			g++;
//		}
//	}
//}

Sample Sampling::uniformSamplingExactGraph(int threshold, int miniter, int maxIter)
{
	int pFreq = 0;
	Graph p = database.initAnyFrequentGraph(threshold, pFreq);
	GraphToMinDFSCode graph2dfs;
	DFSCode p_code;
	graph2dfs.findMinimumDFSCode(&p, p_code);

	if (saveGraph.find(p_code) == saveGraph.end())
	{
		vector<Graph> insop = dualISO.match(database, p);
		saveGraph.assign(p_code, insop, pFreq);
	}

	vector<Graph> upNeighbors;
	vector<Graph> downNeighbors;
	upNeighbors = p.getUpNeigborsExactGraph(database);
	downNeighbors = p.getDownNeighborsExactGraph();

	vector<Graph> neighbors;
	for (vector<Graph>::iterator gr = upNeighbors.begin(); gr != upNeighbors.end(); ++gr)
	{
		GraphToMinDFSCode graph2DFS;
		DFSCode dfs_code;
		graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

		if (saveGraph.find(dfs_code) != saveGraph.end())
		{
			neighbors.push_back(*gr);
		}
		else
		{
			if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int freq_ = computeFrequency(insts);
				if (freq_ >= threshold)
				{
					neighbors.push_back(*gr);
					saveGraph.assign(dfs_code, insts, freq_);
				}
				else
				{
					noneFrequentGraph.insert(dfs_code);
				}
			}
		}
	}

	for (vector<Graph>::iterator gr = downNeighbors.begin(); gr != downNeighbors.end(); ++gr)
	{
		GraphToMinDFSCode graph2DFS;
		DFSCode dfs_code;
		graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

		if (saveGraph.find(dfs_code) != saveGraph.end())
		{
			neighbors.push_back(*gr);
		}
		else
		{
			if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int freq_ = computeFrequency(insts);
				if (freq_ >= threshold)
				{
					neighbors.push_back(*gr);
					saveGraph.assign(dfs_code, insts, freq_);
				}
				else
				{
					noneFrequentGraph.insert(dfs_code);
				}
			}
		}
	}

	std::random_shuffle(neighbors.begin(), neighbors.end());

	int dp = (int) neighbors.size();

	if (dp == 0)
	{
		Sample s;
		s.status = false;
		return s;
	}

	int iter = 0;
	int g = 0;
	int previousSelect = -1;
	Graph q(database.directed);
	vector<Graph> tmpNeighbors;

	while (true)
	{
		double rnd = rand() / (1.0 * RAND_MAX);
		int indxSelected = (int) (rnd * (dp - 1) + 0.5);

		if (indxSelected != previousSelect)
		{
			q.clear();
			q = neighbors[indxSelected];

			previousSelect = indxSelected;
			tmpNeighbors.clear();
			
			upNeighbors.clear();
			downNeighbors.clear();
			//cout << "Getting Up" << endl;
			upNeighbors = q.getUpNeigborsExactGraph(database);
			//cout << "Getting Down" << endl;
			downNeighbors = q.getDownNeighborsExactGraph();

			for (vector<Graph>::iterator gr = upNeighbors.begin(); gr != upNeighbors.end(); ++gr)
			{
				GraphToMinDFSCode graph2DFS;
				DFSCode dfs_code;
				graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

				if (saveGraph.find(dfs_code) != saveGraph.end())
				{
					tmpNeighbors.push_back(*gr);
				}
				else
				{
					if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int freq_ = computeFrequency(insts);
						if (freq_ >= threshold)
						{
							tmpNeighbors.push_back(*gr);
							saveGraph.assign(dfs_code, insts, freq_);
						}
						else
						{
							noneFrequentGraph.insert(dfs_code);
						}
					}
				}
			}

			for (vector<Graph>::iterator gr = downNeighbors.begin(); gr != downNeighbors.end(); ++gr)
			{
				GraphToMinDFSCode graph2DFS;
				DFSCode dfs_code;
				graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

				if (saveGraph.find(dfs_code) != saveGraph.end())
				{
					tmpNeighbors.push_back(*gr);
				}
				else
				{
					if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int freq_ = computeFrequency(insts);
						if (freq_ >= threshold)
						{
							tmpNeighbors.push_back(*gr);
							saveGraph.assign(dfs_code, insts, freq_);
						}
						else
						{
							noneFrequentGraph.insert(dfs_code);
						}
					}
				}
			}
			std::random_shuffle(tmpNeighbors.begin(), tmpNeighbors.end());
		}

		int dq = (int) tmpNeighbors.size();
		double accept_prob = 1;
		double tmp = (dp * 1.0 / dq);
		if (tmp < accept_prob)
		{
			accept_prob = tmp;
		}

		rnd = rand() / (1.0 * RAND_MAX);
		//cout << "Accepted Rate: " << accept_prob << endl;
		if (rnd <= accept_prob || g > maxIter)
		{
			//cout << "One sample is selected" << endl;
			p.clear();
			p = q;
			previousSelect = -1;
			++iter;
			g = 0;

			if (iter >= miniter)
			{
				Sample s;
				s.p = p;
				GraphToMinDFSCode graph2DFS;
				graph2DFS.findMinimumDFSCode(&p, s.dfs_code);
				Hashtable::iterator itFind = saveGraph.find(s.dfs_code);
				s.isomophisms = itFind->second.graphs;
				s.freq = itFind->second.freq;
				for (unsigned int tt = 0; tt < s.isomophisms.size(); tt++)
				{
					s.isomophisms[tt].idGraph = id;
					++id;
				}
				s.status = true;

				return s;
			}
			else
			{
				//cout << "Next" << endl;
				neighbors.clear();
				neighbors = tmpNeighbors;
				dp = (int) tmpNeighbors.size();
			}
		}
		else
		{
			g++;
		}
	}
}

Sample Sampling::uniformSamplingInducedGraph(int threshold, int miniter, int maxIter)
{
	int pFreq = 0;
	Graph p = database.initAnyFrequentGraph(threshold, pFreq);
	GraphToMinDFSCode graph2dfs;
	DFSCode p_code;
	graph2dfs.findMinimumDFSCode(&p, p_code);

	if (saveGraph.find(p_code) == saveGraph.end())
	{
		vector<Graph> insop = dualISO.match(database, p);
		saveGraph.assign(p_code, insop, pFreq);
	}

	vector<Graph> upNeighbors;
	vector<Graph> downNeighbors;
	upNeighbors = p.getUpNeighborsInducedGraph(database);
	downNeighbors = p.getDownNeighborsInducedGraph();

	vector<Graph> neighbors;
	for (vector<Graph>::iterator gr = upNeighbors.begin(); gr != upNeighbors.end(); ++gr)
	{
		GraphToMinDFSCode graph2DFS;
		DFSCode dfs_code;
		graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

		if (saveGraph.find(dfs_code) != saveGraph.end())
		{
			neighbors.push_back(*gr);
		}
		else
		{
			if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int freq_ = computeFrequency(insts);
				if (freq_ >= threshold)
				{
					neighbors.push_back(*gr);
					saveGraph.assign(dfs_code, insts, freq_);
				}
				else
				{
					noneFrequentGraph.insert(dfs_code);
				}
			}
		}
	}

	for (vector<Graph>::iterator gr = downNeighbors.begin(); gr != downNeighbors.end(); ++gr)
	{
		GraphToMinDFSCode graph2DFS;
		DFSCode dfs_code;
		graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

		if (saveGraph.find(dfs_code) != saveGraph.end())
		{
			neighbors.push_back(*gr);
		}
		else
		{
			if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int freq_ = computeFrequency(insts);
				if (freq_ >= threshold)
				{
					neighbors.push_back(*gr);
					saveGraph.assign(dfs_code, insts, freq_);
				}
				else
				{
					noneFrequentGraph.insert(dfs_code);
				}
			}
		}
	}

	std::random_shuffle(neighbors.begin(), neighbors.end());

	int dp = (int) neighbors.size();

	if (dp == 0)
	{
		Sample s;
		s.status = false;
		return s;
	}

	int iter = 0;
	int g = 0;
	int previousSelect = -1;
	Graph q(database.directed);
	vector<Graph> tmpNeighbors;

	while (true)
	{
		double rnd = rand() / (1.0 * RAND_MAX);
		int indxSelected = (int) (rnd * (dp - 1) + 0.5);

		if (indxSelected != previousSelect)
		{
			q.clear();
			q = neighbors[indxSelected];

			previousSelect = indxSelected;
			tmpNeighbors.clear();
			
			upNeighbors.clear();
			downNeighbors.clear();
			//cout << "Getting Up" << endl;
			upNeighbors = q.getUpNeighborsInducedGraph(database);
			//cout << "Getting Down" << endl;
			downNeighbors = q.getDownNeighborsInducedGraph();

			for (vector<Graph>::iterator gr = upNeighbors.begin(); gr != upNeighbors.end(); ++gr)
			{
				GraphToMinDFSCode graph2DFS;
				DFSCode dfs_code;
				graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

				if (saveGraph.find(dfs_code) != saveGraph.end())
				{
					tmpNeighbors.push_back(*gr);
				}
				else
				{
					if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int freq_ = computeFrequency(insts);
						if (freq_ >= threshold)
						{
							tmpNeighbors.push_back(*gr);
							saveGraph.assign(dfs_code, insts, freq_);
						}
						else
						{
							noneFrequentGraph.insert(dfs_code);
						}
					}
				}
			}

			for (vector<Graph>::iterator gr = downNeighbors.begin(); gr != downNeighbors.end(); ++gr)
			{
				GraphToMinDFSCode graph2DFS;
				DFSCode dfs_code;
				graph2DFS.findMinimumDFSCode(&(*gr), dfs_code);

				if (saveGraph.find(dfs_code) != saveGraph.end())
				{
					tmpNeighbors.push_back(*gr);
				}
				else
				{
					if (noneFrequentGraph.find(dfs_code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int freq_ = computeFrequency(insts);
						if (freq_ >= threshold)
						{
							tmpNeighbors.push_back(*gr);
							saveGraph.assign(dfs_code, insts, freq_);
						}
						else
						{
							noneFrequentGraph.insert(dfs_code);
						}
					}
				}
			}
			std::random_shuffle(tmpNeighbors.begin(), tmpNeighbors.end());
		}

		int dq = (int) tmpNeighbors.size();
		double accept_prob = 1;
		double tmp = (dp * 1.0 / dq);
		if (tmp < accept_prob)
		{
			accept_prob = tmp;
		}

		rnd = rand() / (1.0 * RAND_MAX);
		//cout << "Accepted Rate: " << accept_prob << endl;
		if (rnd <= accept_prob || g > maxIter)
		{
			//cout << "One sample is selected" << endl;
			p.clear();
			p = q;
			previousSelect = -1;
			++iter;
			g = 0;

			if (iter >= miniter)
			{
				Sample s;
				s.p = p;
				GraphToMinDFSCode graph2DFS;
				graph2DFS.findMinimumDFSCode(&p, s.dfs_code);
				Hashtable::iterator itFind = saveGraph.find(s.dfs_code);
				s.isomophisms = itFind->second.graphs;
				s.freq = itFind->second.freq;
				for (unsigned int tt = 0; tt < s.isomophisms.size(); tt++)
				{
					s.isomophisms[tt].idGraph = id;
					++id;
				}
				s.status = true;

				return s;
			}
			else
			{
				//cout << "Next" << endl;
				neighbors.clear();
				neighbors = tmpNeighbors;
				dp = (int) tmpNeighbors.size();
			}
		}
		else
		{
			g++;
		}
	}
}

void Sampling::computeCorrelatedValueUniformSamplingExactGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration)
{
	table.clear();
	TopKQueue topKqueue(k);

	clock_t start, end;

	start = clock();

	int threshold;

	for (int g = 0; g < maxIteration; g++)
	{
		if (topKqueue.isFull())
			threshold = topKqueue.minCorrelatedValue();
		else
			threshold = theta;

		//cout << "Start" << endl;
		Sample sam = uniformSamplingExactGraph(threshold, miniter, miniter);
		//cout << "End" << endl;

		if (sam.status == true)
		{
			// compute correlated graph values of sam with other graphs in the hashtable

			if (table.find(sam.dfs_code) == table.end())
			{
				Instance newIns;
				newIns.graphs = sam.isomophisms;
				newIns.freq = sam.freq;

				vector<bool> mark(table.size(), false);
				int markIndx = 0;

				if (table.size() > 1)
				{
					// find child graphs of sam to get ignore list (two graph with all instances having the more than 1 same vertex)
					unsigned int numChild = 0;

					for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
					{
						bool isChild = false;
						if (it->second.graphs[0].edge_size() > sam.p.edge_size())
						{
							isChild = dualISO.isChild(it->second.graphs[0], sam.p);
						}
						else
						{
							isChild = dualISO.isChild(sam.p, it->second.graphs[0]);
							if (isChild)
							{
								newIns.insertIgnoreList(it->second.ignoreList);
							}
						}

						if (isChild)
						{
							mark[markIndx] = true;
							++numChild;
						}
						++markIndx;
					}

					// compute correlated values
					if (numChild < table.size())
					{
						markIndx = 0;
						for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
						{
							if (mark[markIndx] == false)
							{
								if (Utility::isIgnore(it->second, newIns) == false)
								{
									int colocated = 0;
									double confidence = 0;
									int numTestHHop = 0;
									table.computeCorrelatedValue(database, it->second, newIns, colocated, confidence, hop, numTestHHop);

									if (colocated >= theta && confidence >= phi)
									{
										CorrelatedResult res;
										res.g1 = it->second.graphs[0];
										res.g2 = newIns.graphs[0];
										res.colocatedvalue = colocated;
										res.confidencevalue = confidence;
										topKqueue.insert(res);
									}
								}
							}

							++markIndx;
						}
					}
				}

				// Insert sam to hashtable
				table[sam.dfs_code] = newIns;
			}
		}
	}

	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;

	ofstream of;
	of.open(filenameOuput);
	topKqueue.print(of);
	of << endl << "Finished! in " << duration << " (s)";

	of.close();
}

void Sampling::computeCorrelatedValueUniformSamplingInducedGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration)
{
	table.clear();
	TopKQueue topKqueue(k);

	clock_t start, end;

	start = clock();

	int threshold;

	for (int g = 0; g < maxIteration; g++)
	{
		if (topKqueue.isFull())
			threshold = topKqueue.minCorrelatedValue();
		else
			threshold = theta;

		//cout << "Start" << endl;
		Sample sam = uniformSamplingInducedGraph(threshold, miniter, miniter);
		//cout << "End" << endl;

		if (sam.status == true)
		{
			// compute correlated graph values of sam with other graphs in the hashtable

			if (table.find(sam.dfs_code) == table.end())
			{
				Instance newIns;
				newIns.graphs = sam.isomophisms;
				newIns.freq = sam.freq;

				vector<bool> mark(table.size(), false);
				int markIndx = 0;

				if (table.size() > 1)
				{
					// find child graphs of sam to get ignore list (two graph with all instances having the more than 1 same vertex)
					unsigned int numChild = 0;

					for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
					{
						bool isChild = false;
						if (it->second.graphs[0].edge_size() > sam.p.edge_size())
						{
							isChild = dualISO.isChild(it->second.graphs[0], sam.p);
						}
						else
						{
							isChild = dualISO.isChild(sam.p, it->second.graphs[0]);
							if (isChild)
							{
								newIns.insertIgnoreList(it->second.ignoreList);
							}
						}

						if (isChild)
						{
							mark[markIndx] = true;
							++numChild;
						}
						++markIndx;
					}

					// compute correlated values
					if (numChild < table.size())
					{
						markIndx = 0;
						for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
						{
							if (mark[markIndx] == false)
							{
								if (Utility::isIgnore(it->second, newIns) == false)
								{
									int colocated = 0;
									double confidence = 0;
									int numTestHHop = 0;
									table.computeCorrelatedValue(database, it->second, newIns, colocated, confidence, hop, numTestHHop);

									if (colocated >= theta && confidence >= phi)
									{
										CorrelatedResult res;
										res.g1 = it->second.graphs[0];
										res.g2 = newIns.graphs[0];
										res.colocatedvalue = colocated;
										res.confidencevalue = confidence;
										topKqueue.insert(res);
									}
								}
							}

							++markIndx;
						}
					}
				}

				// Insert sam to hashtable
				table[sam.dfs_code] = newIns;
			}
		}
	}

	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;

	ofstream of;
	of.open(filenameOuput);
	topKqueue.print(of);
	of << endl << "Finished! in " << duration << " (s)";

	of.close();
}

Sample Sampling::supportBiasedSamplingExactGraph(int threshold, int miniter, int maxIter)
{
	int supportP = 0;
	double alpha = 0.5;

	Graph p = database.initAnyFrequentGraph(threshold, supportP);
	GraphToMinDFSCode graph2DFSCode;
	DFSCode p_code;
	graph2DFSCode.findMinimumDFSCode(&p, p_code);

	if (saveGraph.find(p_code) == saveGraph.end())
	{
		vector<Graph> isoP = dualISO.match(database, p);
		saveGraph.assign(p_code, isoP, supportP);
	}

	vector<Graph> upNeighbors;
	vector<Graph> downNeighbors;

	vector<int> upSupportNeigbors;
	vector<int> downSupportNeighbors;

	vector<Graph> up = p.getUpNeigborsExactGraph(database);
	vector<Graph> down = p.getDownNeighborsExactGraph();

	for (vector<Graph>::iterator gr = up.begin(); gr != up.end(); ++gr)
	{
		DFSCode code;
		graph2DFSCode.findMinimumDFSCode(&(*gr), code);
		Hashtable::iterator itFind = saveGraph.find(code);
		if (itFind != saveGraph.end())
		{
			upNeighbors.push_back(*gr);
			upSupportNeigbors.push_back(itFind->second.freq);
		}
		else
		{
			if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int support = computeFrequency(insts);
				if (support >= threshold)
				{
					upNeighbors.push_back(*gr);
					upSupportNeigbors.push_back(support);
					saveGraph.assign(code, insts, support);
				}
				else
				{
					noneFrequentGraph.insert(code);
				}
			}
		}
	}

	for (vector<Graph>::iterator gr = down.begin(); gr != down.end(); ++gr)
	{
		DFSCode code;
		graph2DFSCode.findMinimumDFSCode(&(*gr), code);
		Hashtable::iterator itFind = saveGraph.find(code);
		if (itFind != saveGraph.end())
		{
			downNeighbors.push_back(*gr);
			downSupportNeighbors.push_back(itFind->second.freq);
		}
		else
		{
			if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int support = computeFrequency(insts);
				if (support >= threshold)
				{
					downNeighbors.push_back(*gr);
					downSupportNeighbors.push_back(support);
					saveGraph.assign(code, insts, support);
				}
				else
				{
					noneFrequentGraph.insert(code);
				}
			}
		}
	}

	int iter = 0;
	int g = 0;

	while (true)
	{
		double acceptRateMax = 0;
		Graph q;
		vector<Graph> tmpDownNeighbors;
		vector<Graph> tmpUpNeighbors;
		vector<int> tmpUpSupportNeighbors;
		vector<int> tmpDownSupportNeighbors;

		int supportQ;
		double PuvUp;
		double PuvDown;
		double thr;

		if (upNeighbors.size() > 0 && downNeighbors.size() > 0)
		{
			PuvUp = alpha / upNeighbors.size();
			PuvDown = (1 - alpha) / downNeighbors.size();
			thr = alpha;
		}
		else
		{
			if (upNeighbors.size() == 0 && downNeighbors.size() > 0)
			{
				PuvUp = 0;
				PuvDown = (1 - alpha) / downNeighbors.size();
				thr = -1;
			}
			else if (upNeighbors.size() > 0 && downNeighbors.size() == 0)
			{
				PuvDown = 0;
				PuvUp = alpha / upNeighbors.size();
				thr = 1;
			}
			else
			{
				Sample s;
				s.status = false;
				return s;
			}
		}

		double rnd = rand() / (1.0 * RAND_MAX);
		int t = upNeighbors.size();
		int v = downNeighbors.size();
		if (rnd <= thr)
		{
			// Select Up
			double rnd1 = rand() / (1.0 * RAND_MAX);
			int indxSelected = (int) (rnd1 * ((int)upNeighbors.size() - 1) + 0.5);
			q = upNeighbors[indxSelected];
			supportQ = upSupportNeigbors[indxSelected];

			double savedAlpha = alpha;
			down.clear();
			down = q.getDownNeighborsExactGraph();

			tmpDownNeighbors.clear();
			tmpDownSupportNeighbors.clear();

			for (vector<Graph>::iterator gr = down.begin(); gr != down.end(); ++gr)
			{
				DFSCode code;
				graph2DFSCode.findMinimumDFSCode(&(*gr), code);
				Hashtable::iterator itFind = saveGraph.find(code);
				if (itFind != saveGraph.end())
				{
					tmpDownNeighbors.push_back(*gr);
					tmpDownSupportNeighbors.push_back(itFind->second.freq);
				}
				else
				{
					if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int support = computeFrequency(insts);
						if (support >= threshold)
						{
							tmpDownNeighbors.push_back(*gr);
							tmpDownSupportNeighbors.push_back(support);
							saveGraph.assign(code, insts, support);
						}
						else
						{
							noneFrequentGraph.insert(code);
						}
					}
				}
			}

			// find alpha to maximize accept rate
			acceptRateMax = 0;
			for (double b = 0.3; b <= 0.5; b += 0.01)
			{
				double PvuDown = (1 - b) / tmpDownNeighbors.size();
				double acceptRate = supportQ * PvuDown / (supportP * b * (PuvUp / savedAlpha));
				if (acceptRateMax < acceptRate)
				{
					acceptRateMax = acceptRate;
					alpha = b;
				}
			}
		}
		else
		{
			// Select Down
			double rnd1 = rand() / (1.0 * RAND_MAX);
			int indxSelected = (int) (rnd1 * ((int)downNeighbors.size() - 1) + 0.5);
			q = downNeighbors[indxSelected];
			supportQ = downSupportNeighbors[indxSelected];

			double savedAlpha = alpha;
			up.clear();
			up = q.getUpNeigborsExactGraph(database);

			tmpUpNeighbors.clear();
			tmpUpSupportNeighbors.clear();
			for (vector<Graph>::iterator gr = up.begin(); gr != up.end(); ++gr)
			{
				DFSCode code;
				graph2DFSCode.findMinimumDFSCode(&(*gr), code);
				Hashtable::iterator itFind = saveGraph.find(code);
				if (itFind != saveGraph.end())
				{
					tmpUpNeighbors.push_back(*gr);
					tmpUpSupportNeighbors.push_back(itFind->second.freq);
				}
				else
				{
					if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int support = computeFrequency(insts);
						if (support >= threshold)
						{
							tmpUpNeighbors.push_back(*gr);
							tmpUpSupportNeighbors.push_back(support);
							saveGraph.assign(code, insts, support);
						}
						else
						{
							noneFrequentGraph.insert(code);
						}
					}
				}
			}

			// find alpha to maximize accept rate
			acceptRateMax = 0;
			for (double b = 0.3; b <= 0.5; b += 0.01)
			{
				double PvuUp = b / tmpUpNeighbors.size();
				double acceptRate = supportQ * PvuUp / (supportP * (1 - b) * (PuvDown / savedAlpha));
				if (acceptRateMax < acceptRate)
				{
					acceptRateMax = acceptRate;
					alpha = b;
				}
			}
		}

		double rndSe = rand() / (1.0 * RAND_MAX);
		if (rndSe <= acceptRateMax || g > maxIter)
		{
			p.clear();
			p = q;

			++iter;
			g = 0;

			if (iter >= miniter)
			{
				Sample s;
				s.p = p;
				graph2DFSCode.findMinimumDFSCode(&p, s.dfs_code);
				Hashtable::iterator itFind = saveGraph.find(s.dfs_code);
				s.isomophisms = itFind->second.graphs;
				s.freq = itFind->second.freq;
				for (unsigned int tt = 0; tt < s.isomophisms.size(); tt++)
				{
					s.isomophisms[tt].idGraph = id;
					++id;
				}
				s.status = true;

				return s;
			}
			else
			{
				if (tmpUpNeighbors.size() == 0)
				{
					up.clear();
					up = q.getUpNeigborsExactGraph(database);

					tmpUpNeighbors.clear();
					tmpUpSupportNeighbors.clear();

					for (vector<Graph>::iterator gr = up.begin(); gr != up.end(); ++gr)
					{
						DFSCode code;
						graph2DFSCode.findMinimumDFSCode(&(*gr), code);
						Hashtable::iterator itFind = saveGraph.find(code);
						if (itFind != saveGraph.end())
						{
							tmpUpNeighbors.push_back(*gr);
							tmpUpSupportNeighbors.push_back(itFind->second.freq);
						}
						else
						{
							if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
							{
								vector<Graph> insts = dualISO.match(database, *gr);
								int support = computeFrequency(insts);
								if (support >= threshold)
								{
									tmpUpNeighbors.push_back(*gr);
									tmpUpSupportNeighbors.push_back(support);
									saveGraph.assign(code, insts, support);
								}
								else
								{
									noneFrequentGraph.insert(code);
								}
							}
						}
					}
				}

				if (tmpDownNeighbors.size() == 0)
				{
					down.clear();
					down = q.getDownNeighborsExactGraph();

					tmpDownNeighbors.clear();
					tmpDownSupportNeighbors.clear();
					for (vector<Graph>::iterator gr = down.begin(); gr != down.end(); ++gr)
					{
						DFSCode code;
						graph2DFSCode.findMinimumDFSCode(&(*gr), code);
						Hashtable::iterator itFind = saveGraph.find(code);
						if (itFind != saveGraph.end())
						{
							tmpDownNeighbors.push_back(*gr);
							tmpDownSupportNeighbors.push_back(itFind->second.freq);
						}
						else
						{
							if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
							{
								vector<Graph> insts = dualISO.match(database, *gr);
								int support = computeFrequency(insts);
								if (support >= threshold)
								{
									tmpDownNeighbors.push_back(*gr);
									tmpDownSupportNeighbors.push_back(support);
									saveGraph.assign(code, insts, support);
								}
								else
								{
									noneFrequentGraph.insert(code);
								}
							}
						}
					}
				}
				upNeighbors.clear();
				downNeighbors.clear();
				upNeighbors = tmpUpNeighbors;
				downNeighbors = tmpDownNeighbors;
				upSupportNeigbors.clear();
				downSupportNeighbors.clear();
				upSupportNeigbors = tmpUpSupportNeighbors;
				downSupportNeighbors = tmpDownSupportNeighbors;
				supportP = supportQ;
			}
		}
		else
		{
			g++;
		}
	}
}

Sample Sampling::supportBiasedSamplingInducedGraph(int threshold, int miniter, int maxIter)
{
	int supportP = 0;
	double alpha = 0.5;

	Graph p = database.initAnyFrequentGraph(threshold, supportP);
	GraphToMinDFSCode graph2DFSCode;
	DFSCode p_code;
	graph2DFSCode.findMinimumDFSCode(&p, p_code);

	if (saveGraph.find(p_code) == saveGraph.end())
	{
		vector<Graph> isoP = dualISO.match(database, p);
		saveGraph.assign(p_code, isoP, supportP);
	}

	vector<Graph> upNeighbors;
	vector<Graph> downNeighbors;

	vector<int> upSupportNeigbors;
	vector<int> downSupportNeighbors;

	vector<Graph> up = p.getUpNeighborsInducedGraph(database);
	vector<Graph> down = p.getDownNeighborsInducedGraph();

	for (vector<Graph>::iterator gr = up.begin(); gr != up.end(); ++gr)
	{
		DFSCode code;
		graph2DFSCode.findMinimumDFSCode(&(*gr), code);
		Hashtable::iterator itFind = saveGraph.find(code);
		if (itFind != saveGraph.end())
		{
			upNeighbors.push_back(*gr);
			upSupportNeigbors.push_back(itFind->second.freq);
		}
		else
		{
			if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int support = computeFrequency(insts);
				if (support >= threshold)
				{
					upNeighbors.push_back(*gr);
					upSupportNeigbors.push_back(support);
					saveGraph.assign(code, insts, support);
				}
				else
				{
					noneFrequentGraph.insert(code);
				}
			}
		}
	}

	for (vector<Graph>::iterator gr = down.begin(); gr != down.end(); ++gr)
	{
		DFSCode code;
		graph2DFSCode.findMinimumDFSCode(&(*gr), code);
		Hashtable::iterator itFind = saveGraph.find(code);
		if (itFind != saveGraph.end())
		{
			downNeighbors.push_back(*gr);
			downSupportNeighbors.push_back(itFind->second.freq);
		}
		else
		{
			if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
			{
				vector<Graph> insts = dualISO.match(database, *gr);
				int support = computeFrequency(insts);
				if (support >= threshold)
				{
					downNeighbors.push_back(*gr);
					downSupportNeighbors.push_back(support);
					saveGraph.assign(code, insts, support);
				}
				else
				{
					noneFrequentGraph.insert(code);
				}
			}
		}
	}

	int iter = 0;
	int g = 0;

	while (true)
	{
		double acceptRateMax = 0;
		Graph q;
		vector<Graph> tmpDownNeighbors;
		vector<Graph> tmpUpNeighbors;
		vector<int> tmpUpSupportNeighbors;
		vector<int> tmpDownSupportNeighbors;

		int supportQ;
		double PuvUp;
		double PuvDown;
		double thr;

		if (upNeighbors.size() > 0 && downNeighbors.size() > 0)
		{
			PuvUp = alpha / upNeighbors.size();
			PuvDown = (1 - alpha) / downNeighbors.size();
			thr = alpha;
		}
		else
		{
			if (upNeighbors.size() == 0 && downNeighbors.size() > 0)
			{
				PuvUp = 0;
				PuvDown = (1 - alpha) / downNeighbors.size();
				thr = -1;
			}
			else if (upNeighbors.size() > 0 && downNeighbors.size() == 0)
			{
				PuvDown = 0;
				PuvUp = alpha / upNeighbors.size();
				thr = 1;
			}
			else
			{
				Sample s;
				s.status = false;
				return s;
			}
		}

		double rnd = rand() / (1.0 * RAND_MAX);

		if (rnd <= thr)
		{
			// Select Up
			double rnd1 = rand() / (1.0 * RAND_MAX);
			int indxSelected = (int) (rnd1 * ((int)upNeighbors.size() - 1) + 0.5);
			q = upNeighbors[indxSelected];
			supportQ = upSupportNeigbors[indxSelected];

			double savedAlpha = alpha;
			down.clear();
			down = q.getDownNeighborsInducedGraph();

			tmpDownNeighbors.clear();
			tmpDownSupportNeighbors.clear();

			for (vector<Graph>::iterator gr = down.begin(); gr != down.end(); ++gr)
			{
				DFSCode code;
				graph2DFSCode.findMinimumDFSCode(&(*gr), code);
				Hashtable::iterator itFind = saveGraph.find(code);
				if (itFind != saveGraph.end())
				{
					tmpDownNeighbors.push_back(*gr);
					tmpDownSupportNeighbors.push_back(itFind->second.freq);
				}
				else
				{
					if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int support = computeFrequency(insts);
						if (support >= threshold)
						{
							tmpDownNeighbors.push_back(*gr);
							tmpDownSupportNeighbors.push_back(support);
							saveGraph.assign(code, insts, support);
						}
						else
						{
							noneFrequentGraph.insert(code);
						}
					}
				}
			}

			// find alpha to maximize accept rate
			acceptRateMax = 0;
			for (double b = 0.3; b <= 0.5; b += 0.01)
			{
				double PvuDown = (1 - b) / tmpDownNeighbors.size();
				double acceptRate = supportQ * PvuDown / (supportP * b * (PuvUp / savedAlpha));
				if (acceptRateMax < acceptRate)
				{
					acceptRateMax = acceptRate;
					alpha = b;
				}
			}
		}
		else
		{
			// Select Down
			double rnd1 = rand() / (1.0 * RAND_MAX);
			int indxSelected = (int) (rnd1 * ((int)downNeighbors.size() - 1) + 0.5);
			q = downNeighbors[indxSelected];
			supportQ = downSupportNeighbors[indxSelected];

			double savedAlpha = alpha;
			up.clear();
			up = q.getUpNeighborsInducedGraph(database);

			tmpUpNeighbors.clear();
			tmpUpSupportNeighbors.clear();
			for (vector<Graph>::iterator gr = up.begin(); gr != up.end(); ++gr)
			{
				DFSCode code;
				graph2DFSCode.findMinimumDFSCode(&(*gr), code);
				Hashtable::iterator itFind = saveGraph.find(code);
				if (itFind != saveGraph.end())
				{
					tmpUpNeighbors.push_back(*gr);
					tmpUpSupportNeighbors.push_back(itFind->second.freq);
				}
				else
				{
					if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
					{
						vector<Graph> insts = dualISO.match(database, *gr);
						int support = computeFrequency(insts);
						if (support >= threshold)
						{
							tmpUpNeighbors.push_back(*gr);
							tmpUpSupportNeighbors.push_back(support);
							saveGraph.assign(code, insts, support);
						}
						else
						{
							noneFrequentGraph.insert(code);
						}
					}
				}
			}

			// find alpha to maximize accept rate
			acceptRateMax = 0;
			for (double b = 0.3; b <= 0.5; b += 0.01)
			{
				double PvuUp = b / tmpUpNeighbors.size();
				double acceptRate = supportQ * PvuUp / (supportP * (1 - b) * (PuvDown / savedAlpha));
				if (acceptRateMax < acceptRate)
				{
					acceptRateMax = acceptRate;
					alpha = b;
				}
			}
		}

		double rndSe = rand() / (1.0 * RAND_MAX);
		if (rndSe <= acceptRateMax || g > maxIter)
		{
			p.clear();
			p = q;

			++iter;
			g = 0;

			if (iter >= miniter)
			{
				Sample s;
				s.p = p;
				graph2DFSCode.findMinimumDFSCode(&p, s.dfs_code);
				Hashtable::iterator itFind = saveGraph.find(s.dfs_code);
				s.isomophisms = itFind->second.graphs;
				s.freq = itFind->second.freq;
				for (unsigned int tt = 0; tt < s.isomophisms.size(); tt++)
				{
					s.isomophisms[tt].idGraph = id;
					++id;
				}
				s.status = true;

				return s;
			}
			else
			{
				if (tmpUpNeighbors.size() == 0)
				{
					up.clear();
					up = q.getUpNeighborsInducedGraph(database);

					tmpUpNeighbors.clear();
					tmpUpSupportNeighbors.clear();

					for (vector<Graph>::iterator gr = up.begin(); gr != up.end(); ++gr)
					{
						DFSCode code;
						graph2DFSCode.findMinimumDFSCode(&(*gr), code);
						Hashtable::iterator itFind = saveGraph.find(code);
						if (itFind != saveGraph.end())
						{
							tmpUpNeighbors.push_back(*gr);
							tmpUpSupportNeighbors.push_back(itFind->second.freq);
						}
						else
						{
							if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
							{
								vector<Graph> insts = dualISO.match(database, *gr);
								int support = computeFrequency(insts);
								if (support >= threshold)
								{
									tmpUpNeighbors.push_back(*gr);
									tmpUpSupportNeighbors.push_back(support);
									saveGraph.assign(code, insts, support);
								}
								else
								{
									noneFrequentGraph.insert(code);
								}
							}
						}
					}
				}

				if (tmpDownNeighbors.size() == 0)
				{
					down.clear();
					down = q.getDownNeighborsInducedGraph();

					tmpDownNeighbors.clear();
					tmpDownSupportNeighbors.clear();
					for (vector<Graph>::iterator gr = down.begin(); gr != down.end(); ++gr)
					{
						DFSCode code;
						graph2DFSCode.findMinimumDFSCode(&(*gr), code);
						Hashtable::iterator itFind = saveGraph.find(code);
						if (itFind != saveGraph.end())
						{
							tmpDownNeighbors.push_back(*gr);
							tmpDownSupportNeighbors.push_back(itFind->second.freq);
						}
						else
						{
							if (noneFrequentGraph.find(code) == noneFrequentGraph.end())
							{
								vector<Graph> insts = dualISO.match(database, *gr);
								int support = computeFrequency(insts);
								if (support >= threshold)
								{
									tmpDownNeighbors.push_back(*gr);
									tmpDownSupportNeighbors.push_back(support);
									saveGraph.assign(code, insts, support);
								}
								else
								{
									noneFrequentGraph.insert(code);
								}
							}
						}
					}
				}
				upNeighbors.clear();
				downNeighbors.clear();
				upNeighbors = tmpUpNeighbors;
				downNeighbors = tmpDownNeighbors;
				upSupportNeigbors.clear();
				downSupportNeighbors.clear();
				upSupportNeigbors = tmpUpSupportNeighbors;
				downSupportNeighbors = tmpDownSupportNeighbors;
				supportP = supportQ;
			}
		}
		else
		{
			g++;
		}
	}
}

void Sampling::computeCorrelatedValueSupportBiasedSamplingExactGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration)
{
	table.clear();
	TopKQueue topKqueue(k);

	clock_t start, end;

	start = clock();

	int threshold;

	for (int g = 0; g < maxIteration; g++)
	{
		if (topKqueue.isFull())
			threshold = topKqueue.minCorrelatedValue();
		else
			threshold = theta;

		//cout << "Start" << endl;
		Sample sam = supportBiasedSamplingExactGraph(threshold, miniter, miniter);
		//cout << "End" << endl;

		if (sam.status == true)
		{
			// compute correlated graph values of sam with other graphs in the hashtable

			if (table.find(sam.dfs_code) == table.end())
			{
				Instance newIns;
				newIns.graphs = sam.isomophisms;
				newIns.freq = sam.freq;

				vector<bool> mark(table.size(), false);
				int markIndx = 0;

				if (table.size() > 1)
				{
					// find child graphs of sam to get ignore list (two graph with all instances having the more than 1 same vertex)
					unsigned int numChild = 0;

					for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
					{
						bool isChild = false;
						if (it->second.graphs[0].edge_size() > sam.p.edge_size())
						{
							isChild = dualISO.isChild(it->second.graphs[0], sam.p);
						}
						else
						{
							isChild = dualISO.isChild(sam.p, it->second.graphs[0]);
							if (isChild)
							{
								newIns.insertIgnoreList(it->second.ignoreList);
							}
						}

						if (isChild)
						{
							mark[markIndx] = true;
							++numChild;
						}
						++markIndx;
					}

					// compute correlated values
					if (numChild < table.size())
					{
						markIndx = 0;
						for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
						{
							if (mark[markIndx] == false)
							{
								if (Utility::isIgnore(it->second, newIns) == false)
								{
									int colocated = 0;
									double confidence = 0;
									int numTestHHop = 0;
									table.computeCorrelatedValue(database, it->second, newIns, colocated, confidence, hop, numTestHHop);

									if (colocated >= theta && confidence >= phi)
									{
										CorrelatedResult res;
										res.g1 = it->second.graphs[0];
										res.g2 = newIns.graphs[0];
										res.colocatedvalue = colocated;
										res.confidencevalue = confidence;
										topKqueue.insert(res);
									}
								}
							}

							++markIndx;
						}
					}
				}

				// Insert sam to hashtable
				table[sam.dfs_code] = newIns;
			}
		}
	}

	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;

	ofstream of;
	of.open(filenameOuput);
	topKqueue.print(of);
	of << endl << "Finished! in " << duration << " (s)";

	of.close();
}

void Sampling::computeCorrelatedValueSupportBiasedSamplingInducedGraph(char* filenameOuput, int theta, double phi, int hop, int k, int miniter, int maxIteration)
{
	table.clear();
	TopKQueue topKqueue(k);

	clock_t start, end;

	start = clock();

	int threshold;

	for (int g = 0; g < maxIteration; g++)
	{
		if (topKqueue.isFull())
			threshold = topKqueue.minCorrelatedValue();
		else
			threshold = theta;

		//cout << "Start" << endl;
		Sample sam = supportBiasedSamplingInducedGraph(threshold, miniter, miniter);
		//cout << "End" << endl;

		if (sam.status == true)
		{
			// compute correlated graph values of sam with other graphs in the hashtable

			if (table.find(sam.dfs_code) == table.end())
			{
				Instance newIns;
				newIns.graphs = sam.isomophisms;
				newIns.freq = sam.freq;

				vector<bool> mark(table.size(), false);
				int markIndx = 0;

				if (table.size() > 1)
				{
					// find child graphs of sam to get ignore list (two graph with all instances having the more than 1 same vertex)
					unsigned int numChild = 0;

					for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
					{
						bool isChild = false;
						if (it->second.graphs[0].edge_size() > sam.p.edge_size())
						{
							isChild = dualISO.isChild(it->second.graphs[0], sam.p);
						}
						else
						{
							isChild = dualISO.isChild(sam.p, it->second.graphs[0]);
							if (isChild)
							{
								newIns.insertIgnoreList(it->second.ignoreList);
							}
						}

						if (isChild)
						{
							mark[markIndx] = true;
							++numChild;
						}
						++markIndx;
					}

					// compute correlated values
					if (numChild < table.size())
					{
						markIndx = 0;
						for (Hashtable::hastable_iterator it = table.begin(); it != table.end(); it++)
						{
							if (mark[markIndx] == false)
							{
								if (Utility::isIgnore(it->second, newIns) == false)
								{
									int colocated = 0;
									double confidence = 0;
									int numTestHHop = 0;
									table.computeCorrelatedValue(database, it->second, newIns, colocated, confidence, hop, numTestHHop);

									if (colocated >= theta && confidence >= phi)
									{
										CorrelatedResult res;
										res.g1 = it->second.graphs[0];
										res.g2 = newIns.graphs[0];
										res.colocatedvalue = colocated;
										res.confidencevalue = confidence;
										topKqueue.insert(res);
									}
								}
							}

							++markIndx;
						}
					}
				}

				// Insert sam to hashtable
				table[sam.dfs_code] = newIns;
			}
		}
	}

	end = clock();

	double duration = (end-start) * 1.0 / CLOCKS_PER_SEC;

	ofstream of;
	of.open(filenameOuput);
	topKqueue.print(of);
	of << endl << "Finished! in " << duration << " (s)";

	of.close();
}