#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <string>
#include <cstring>
#include <set>
#include <stdlib.h>
#include <algorithm>

using namespace std;

struct ProteinName
{
	unsigned int id;
	string name;
};

struct VertexLabel
{
	unsigned int id;
	string function;
	string symbol;
	set<string> proteinName;

	bool findName(string & n)
	{
		set<string>::iterator itFind = proteinName.find(n);
		if (itFind == proteinName.end())
		{
			return false;
		}

		return true;
	}
};

struct Edge
{
	int idFrom;
	int idTo;

	bool operator == (const Edge & e) const
	{
		return idFrom == e.idFrom && idTo == e.idTo;
	}

	bool operator < (const Edge & e) const
	{
		if (idFrom < e.idFrom)
			return true;
		else if (idFrom == e.idFrom)
		{
			return idTo < e.idTo;
		}
		else
			return false;
	}
};

struct Vertex
{
	int id;
	int label;
	string name;
	set<Edge> edges;

	bool operator == (const Vertex & v) const
	{
		return id == v.id;
	}
};

vector<Vertex> vertices;

vector<VertexLabel> listLabels;
set<string> proteinNames;

int findVertexLabel(string name)
{
	int size = (int) listLabels.size();
	for (int i = 0; i < size; i++)
	{
		if (listLabels[i].findName(name) == true)
		{
			return listLabels[i].id;
		}
	}

	return -1;
}

int findVertex(string name)
{
	int size = (int) vertices.size();
	for (int i = 0; i < size; i++)
	{
		if (vertices[i].name == name)
			return i;
	}

	return -1;
}

void readLinkFile(char * fileLink)
{
	cout << "Read Link File" << endl;

	ifstream in;
	char line[500];
	char* rdStr;

	in.open(fileLink);
	
	int id = 0;

	while(!in.eof())
	{
		in.getline(line,500, '\n');
		if (strlen(line) == 0)
			continue;
		else
		{
			rdStr = strtok(line,". ");
			
			rdStr = strtok(NULL,". ");
			string name1 = string(rdStr);
			
			rdStr = strtok(NULL,". ");

			rdStr = strtok(NULL,". ");
			string name2 = string(rdStr);

			if (proteinNames.find(name1) != proteinNames.end() && proteinNames.find(name2) != proteinNames.end())
			{
				int idFind1 = findVertex(name1);

				if (idFind1 == -1)
				{
					Vertex v;
					v.id = id;
					++id;
					v.name = name1;
					// find label
					v.label = findVertexLabel(name1);
					vertices.push_back(v);
					idFind1 = (int)vertices.size() - 1;
				}

				int idFind2 = findVertex(name2);

				if (idFind2 == -1)
				{
					Vertex v;
					v.id = id;
					++id;
					v.name = name2;
					// find label
					v.label = findVertexLabel(name2);
					vertices.push_back(v);
					idFind2 = (int)vertices.size() - 1;
				}

				Edge e1;
				e1.idFrom = vertices[idFind1].id;
				e1.idTo = vertices[idFind2].id;
				vertices[idFind1].edges.insert(e1);

				Edge e2;
				e2.idFrom = vertices[idFind2].id;
				e2.idTo = vertices[idFind1].id;
				vertices[idFind2].edges.insert(e2);
			}
		}
	}

	in.close();
}

void writeLabelFile(char * fileLabelNameOut)
{
	ofstream os;
	os.open(fileLabelNameOut);

	for (vector<VertexLabel>::iterator it = listLabels.begin(); it != listLabels.end(); ++it)
	{
		os << it->id << " " << it->function << " " << it->symbol << endl;
	}

	os.close();
}

void writeProteinName(char * fileProteinNameOut)
{
	ofstream os;
	os.open(fileProteinNameOut);

	for (vector<Vertex>::iterator it = vertices.begin(); it != vertices.end(); ++it)
	{
		os << it->id << " " << it->name << endl;
	}

	os.close();
}

void readLabelFile (char * labelFileName)
{
	std::cout << "Read Label file" << endl;

	ifstream in;
	char line[50000];
	char* rdStr;

	in.open(labelFileName);
	unsigned int id = 0;

	while(!in.eof())
	{
		in.getline(line,50000, '\n');
		if (strlen(line) == 0)
			continue;
		else
		{
			rdStr = strtok(line,",\t ");
			string ele1 = string(rdStr);

			if (ele1 == "TERM")
			{
				VertexLabel vl;
				vl.id = id;
				++id;

				rdStr = strtok(NULL,",\t ");
				vl.function = string(rdStr);
				listLabels.push_back(vl);
			}
			else if (ele1 == "GOID")
			{
				rdStr = strtok(NULL,",\t ");
				listLabels[listLabels.size() - 1].symbol = string(rdStr);
			}
			else
			{
				listLabels[listLabels.size() - 1].proteinName.insert(ele1);
		
				rdStr = strtok(NULL,",\t ");
				while (rdStr != NULL)
				{
					string na = string(rdStr);
					proteinNames.insert(na);
					listLabels[listLabels.size() - 1].proteinName.insert(na);
					rdStr = strtok(NULL,",\t ");
				}
			}
		}
	}

	in.close();
}

void writeFile(const char * outputFileName)
{
	std::cout << "Write results" << endl;
	
	set<string> tmp;
	
	ofstream os;
	os.open(outputFileName);

	for (unsigned int from = 0; from < vertices.size(); ++from)
	{
		os << "v " << vertices[from].id << " " << vertices[from].label << std::endl;
		
		for(set<Edge>::const_iterator it = vertices[from].edges.begin(); it != vertices[from].edges.end(); ++it)
		{
			char buf[512];
			if(vertices[from].id <= it->idTo)
			{
				sprintf(buf,"%d %d %d", vertices[from].id, it->idTo, 1);
			}
			else
			{
				sprintf(buf,"%d %d %d", it->idTo, vertices[from].id, 1);
			}

			tmp.insert(buf);
		}
	}
	
	for(set<string>::iterator it = tmp.begin(); it != tmp.end(); ++it)
	{
		os << "e " << *it << endl;
	}
	
	//os << "Dupplicate: " << duplicate;
	os.close();
}

int main(int arg, char ** argv)
{
	char labelFileName[150];
	char linkFileName[150];

	cout << "Input name of label file: ";
	cin.getline(labelFileName, 150, '\n');

	cout << "Input link of proteins file name: ";
	cin.getline(linkFileName, 150, '\n');

	char resultFileName[150];
	strcpy(resultFileName, "Result_");
	strcat(resultFileName, linkFileName);

	readLabelFile(labelFileName);
	readLinkFile(linkFileName);
	writeFile(resultFileName);

	char fileProteinNameOut[150];
	strcpy(fileProteinNameOut, "Protein_Mapping_");
	strcat(fileProteinNameOut, linkFileName);
	writeProteinName(fileProteinNameOut);

	char fileLabelOut[150];
	strcpy(fileLabelOut, "Label_Mapping_");
	strcat(fileLabelOut, linkFileName);
	writeLabelFile(fileLabelOut);

	return 0;
}