#include "hashtable.h"

void Hashtable::push(DFSCode& dfs_code, Graph& g, set<DFSCode>& child)
{
	(*this)[dfs_code].push(g, child);
}

void Hashtable::push(DFSCode& dfs_code, Graph& g, set<DFSCode>& child, set<CodeId> & ignore)
{
	(*this)[dfs_code].push(g, child, ignore);
}

void Hashtable::push(DFSCode& dfs_code, Graph& g)
{
	(*this)[dfs_code].push(g);
}

void Hashtable::computeFrequency()
{
	for (Hashtable::hastable_iterator it = this->begin(); it != this->end(); ++it)
	{
		it->second.computeFrequency();
	}
}

void Hashtable::computeFrequency(Hashtable::hastable_iterator start)
{
	for (Hashtable::hastable_iterator it = start; it != this->end(); ++it)
	{
		it->second.computeFrequency();
	}
}

int Hashtable::freq(DFSCode dfs_code)
{
	return (*this)[dfs_code].freq;
}

void Hashtable::computeCorrelatedValue (Graph & bigGraph, Instance & ins1, Instance & ins2, int & colocated, double & confidence, int hop, int& numTestCollocated)
{
	//vector<bool> I1(ins1.independent_graphs.size(), false);
	//vector<bool> I2(ins2.independent_graphs.size(), false);
	vector<bool> I1(ins1.graphs.size(), false);
	vector<bool> I2(ins2.graphs.size(), false);

	/*for (int i = 0; i < ins1.independent_graphs.size(); i++)
	{
		for (int j = 0; j < ins2.independent_graphs.size(); j++)
		{*/

	for (int i = 0; i < ins1.graphs.size(); i++)
	{
		for (int j = 0; j < ins2.graphs.size(); j++)
		{
			++numTestCollocated;

			//if (Utility::isInHHop(bigGraph, ins1.graphs[ins1.independent_graphs[i]], ins2.graphs[ins2.independent_graphs[j]], hop) == true)
			if (Utility::isInHHop(bigGraph, ins1.graphs[i], ins2.graphs[j], hop) == true)
			{
				I1[i] = true;
				I2[j] = true;
				break;
			}
		}
	}

	//for (int i = 0; i < ins2.independent_graphs.size(); i++)
	for (int i = 0; i < ins2.graphs.size(); i++)
	{
		if (I2[i] == false)
		{
			//for (int j = 0; j < ins1.independent_graphs.size(); j++)
			for (int j = 0; j < ins1.graphs.size(); j++)
			{
				++numTestCollocated;

				//if (Utility::isInHHop(bigGraph, ins1.graphs[ins1.independent_graphs[j]], ins2.graphs[ins2.independent_graphs[i]], hop) == true)
				if (Utility::isInHHop(bigGraph, ins1.graphs[j], ins2.graphs[i], hop) == true)
				{
					I2[i] = true;
					break;
				}
			}
		}
	}

	int tmpColo1 = 0, tmpColo2 = 0;
	for (int i = 0; i < I1.size(); i++)
	{
		if (I1[i] == true)
			++tmpColo1;
	}

	if (tmpColo1 > ins1.freq)
		tmpColo1 = ins1.freq;

	for (int i = 0; i < I2.size(); i++)
	{
		if (I2[i] == true)
			++tmpColo2;
	}

	if (tmpColo2 > ins2.freq)
		tmpColo2 = ins2.freq;

	colocated = (tmpColo1 < tmpColo2) ? tmpColo1 : tmpColo2;
	
	int maxFreq = (ins1.freq > ins2.freq) ? ins1.freq : ins2.freq;

	confidence = colocated * 1.0 / maxFreq;
}

//void Hashtable::computeCorrelatedValueClose (Graph & bigGraph, Instance& ins1, Instance& ins2, int & colocated, double & confidence, int hop, int& numTestCollocated)
//{
//	//vector<bool> I1(ins1.independent_graphs.size(), false);
//	//vector<bool> I2(ins2.independent_graphs.size(), false);
//
//	vector<bool> I1(ins1.graphs.size(), false);
//	vector<bool> I2(ins2.graphs.size(), false);
//
//	/*for (int i = 0; i < ins1.independent_graphs.size(); i++)
//	{
//		for (int j = 0; j < ins2.independent_graphs.size(); j++)
//		{*/
//	for (int i = 0; i < ins1.graphs.size(); i++)
//	{
//		for (int j = 0; j < ins2.graphs.size(); j++)
//		{
//			bool isTested = false;
//			//for (set<CodeId>::iterator it1 = ins1.graphs[ins1.independent_graphs[i]].sameHHop.begin(); it1 != ins1.graphs[ins1.independent_graphs[i]].sameHHop.end(); ++it1)
//			for (set<CodeId>::iterator it1 = ins1.graphs[i].sameHHop.begin(); it1 != ins1.graphs[i].sameHHop.end(); ++it1)
//			{
//				/*set<CodeId>::const_iterator got = ins2.graphs[ins2.independent_graphs[j]].sameHHop.find(*it1);
//				if (got != ins2.graphs[ins2.independent_graphs[j]].sameHHop.end())
//				{*/
//				set<CodeId>::const_iterator got = ins2.graphs[j].sameHHop.find(*it1);
//				if (got != ins2.graphs[j].sameHHop.end())
//				{
//					isTested = true;
//					I1[i] = true;
//					I2[j] = true;
//					break;
//				}
//			}
//			
//			if (isTested == false)
//			{
//				++numTestCollocated;
//
//				//if (Utility::isInHHop(bigGraph, ins1.graphs[ins1.independent_graphs[i]], ins2.graphs[ins2.independent_graphs[j]], hop) == true)
//				if (Utility::isInHHop(bigGraph, ins1.graphs[i], ins2.graphs[j], hop) == true)
//				{
//					// create same code ID for each graph
//					CodeId codeId;
//					/*codeId.id1 = ins1.graphs[ins1.independent_graphs[i]].idGraph;
//					codeId.id2 = ins2.graphs[ins2.independent_graphs[j]].idGraph;
//					ins1.graphs[ins1.independent_graphs[i]].sameHHop.insert(codeId);
//					ins2.graphs[ins2.independent_graphs[j]].sameHHop.insert(codeId);*/
//
//					codeId.id1 = ins1.graphs[i].idGraph;
//					codeId.id2 = ins2.graphs[j].idGraph;
//					ins1.graphs[i].sameHHop.insert(codeId);
//					ins2.graphs[j].sameHHop.insert(codeId);
//
//					I1[i] = true;
//					I2[j] = true;
//					break;
//				}
//			}
//			else
//				break;
//		}
//	}
//
//	//for (int i = 0; i < ins2.independent_graphs.size(); i++)
//	for (int i = 0; i < ins2.graphs.size(); i++)
//	{
//		if (I2[i] == false)
//		{
//			//for (int j = 0; j < ins1.independent_graphs.size(); j++)
//			for (int j = 0; j < ins1.graphs.size(); j++)
//			{
//				bool isTested = false;
//				//for (set<CodeId>::iterator it1 = ins1.graphs[ins1.independent_graphs[j]].sameHHop.begin(); it1 != ins1.graphs[ins1.independent_graphs[j]].sameHHop.end(); ++it1)
//				for (set<CodeId>::iterator it1 = ins1.graphs[j].sameHHop.begin(); it1 != ins1.graphs[j].sameHHop.end(); ++it1)
//				{
//					//set<CodeId>::const_iterator got = ins2.graphs[ins2.independent_graphs[i]].sameHHop.find(*it1);
//					set<CodeId>::const_iterator got = ins2.graphs[i].sameHHop.find(*it1);
//					//if (got != ins2.graphs[ins2.independent_graphs[i]].sameHHop.end())
//					if (got != ins2.graphs[i].sameHHop.end())
//					{
//						isTested = true;
//						I2[i] = true;
//						break;
//					}
//				}
//
//				if (isTested == false)
//				{
//					++numTestCollocated;
//
//					//if (Utility::isInHHop(bigGraph, ins1.graphs[ins1.independent_graphs[j]], ins2.graphs[ins2.independent_graphs[i]], hop) == true)
//					if (Utility::isInHHop(bigGraph, ins1.graphs[j], ins2.graphs[i], hop) == true)
//					{
//						// create same code ID for each graph
//						CodeId codeId;
//						/*codeId.id1 = ins1.graphs[ins1.independent_graphs[j]].idGraph;
//						codeId.id2 = ins2.graphs[ins2.independent_graphs[i]].idGraph;
//						ins1.graphs[ins1.independent_graphs[j]].sameHHop.insert(codeId);
//						ins2.graphs[ins2.independent_graphs[i]].sameHHop.insert(codeId);*/
//
//						codeId.id1 = ins1.graphs[j].idGraph;
//						codeId.id2 = ins2.graphs[i].idGraph;
//						ins1.graphs[j].sameHHop.insert(codeId);
//						ins2.graphs[i].sameHHop.insert(codeId);
//
//						I2[i] = true;
//						break;
//					}
//				}
//				else
//					break;
//			}
//		}
//	}
//
//	int tmpColo1 = 0, tmpColo2 = 0;
//	for (int i = 0; i < I1.size(); i++)
//	{
//		if (I1[i] == true)
//			++tmpColo1;
//	}
//	if (tmpColo1 > ins1.freq)
//		tmpColo1 = ins1.freq;
//
//	for (int i = 0; i < I2.size(); i++)
//	{
//		if (I2[i] == true)
//			++tmpColo2;
//	}
//	if (tmpColo2 > ins2.freq)
//		tmpColo2 = ins2.freq;
//
//	colocated = (tmpColo1 < tmpColo2) ? tmpColo1 : tmpColo2;
//	
//	int maxFreq = (ins1.freq > ins2.freq) ? ins1.freq : ins2.freq;
//
//	confidence = colocated * 1.0 / maxFreq;
//}

void Hashtable::computeCorrelatedValueClose (Graph & bigGraph, Instance& ins1, Instance& ins2, int & colocated, double & confidence, int hop, int& numTestCollocated)
{
	vector<bool> I1(ins1.graphs.size(), false);
	vector<bool> I2(ins2.graphs.size(), false);

	for (int i = 0; i < ins1.graphs.size(); i++)
	{
		for (int j = 0; j < ins2.graphs.size(); j++)
		{
			bool isTested = false;
			for (unordered_set<double>::iterator it1 = ins1.graphs[i].sameHHop.begin(); it1 != ins1.graphs[i].sameHHop.end(); ++it1)
			{
				unordered_set<double>::const_iterator got = ins2.graphs[j].sameHHop.find(*it1);
				if (got != ins2.graphs[j].sameHHop.end())
				{
					isTested = true;
					I1[i] = true;
					I2[j] = true;
					break;
				}
			}
			
			if (isTested == false)
			{
				++numTestCollocated;

				if (Utility::isInHHop(bigGraph, ins1.graphs[i], ins2.graphs[j], hop) == true)
				{
					// create same code ID for each graph
					double codeId;
				
					codeId = Utility::pairFuntion(ins1.graphs[i].idGraph, ins2.graphs[j].idGraph);
					ins1.graphs[i].sameHHop.insert(codeId);
					ins2.graphs[j].sameHHop.insert(codeId);

					I1[i] = true;
					I2[j] = true;
					break;
				}
			}
			else
				break;
		}
	}

	for (int i = 0; i < ins2.graphs.size(); i++)
	{
		if (I2[i] == false)
		{
			for (int j = 0; j < ins1.graphs.size(); j++)
			{
				bool isTested = false;
			
				for (unordered_set<double>::iterator it1 = ins1.graphs[j].sameHHop.begin(); it1 != ins1.graphs[j].sameHHop.end(); ++it1)
				{
					unordered_set<double>::const_iterator got = ins2.graphs[i].sameHHop.find(*it1);
					if (got != ins2.graphs[i].sameHHop.end())
					{
						isTested = true;
						I2[i] = true;
						break;
					}
				}

				if (isTested == false)
				{
					++numTestCollocated;

					if (Utility::isInHHop(bigGraph, ins1.graphs[j], ins2.graphs[i], hop) == true)
					{
						// create same code ID for each graph
						double codeId;
						
						codeId = Utility::pairFuntion(ins1.graphs[j].idGraph, ins2.graphs[i].idGraph);
						ins1.graphs[j].sameHHop.insert(codeId);
						ins2.graphs[i].sameHHop.insert(codeId);

						I2[i] = true;
						break;
					}
				}
				else
					break;
			}
		}
	}

	int tmpColo1 = 0, tmpColo2 = 0;
	for (int i = 0; i < I1.size(); i++)
	{
		if (I1[i] == true)
			++tmpColo1;
	}
	if (tmpColo1 > ins1.freq)
		tmpColo1 = ins1.freq;

	for (int i = 0; i < I2.size(); i++)
	{
		if (I2[i] == true)
			++tmpColo2;
	}
	if (tmpColo2 > ins2.freq)
		tmpColo2 = ins2.freq;

	colocated = (tmpColo1 < tmpColo2) ? tmpColo1 : tmpColo2;
	
	int maxFreq = (ins1.freq > ins2.freq) ? ins1.freq : ins2.freq;

	confidence = colocated * 1.0 / maxFreq;
}