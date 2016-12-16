#include "correlatedresult.h"

void CorrelatedResult::report_result(ofstream & of, int id)
{
	of << "Pair " << id << endl;
	of << "Co-located value: " << colocatedvalue << endl;
	of << "Confidence value: " << confidencevalue << endl;
	of << "G1: " << endl;
	g1.write(of);
	of << "G2: " << endl;
	g2.write(of);
	of << endl;
}