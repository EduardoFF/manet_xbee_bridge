#ifndef _TREE_H_
#define _TREE_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <map>
#include <vector>

struct EntryData
{
	string finalDest;
	string destNode;
	int weight;
};

int main()
{
	vector <EntryData> entry;
	entry.push_back(EntryData());
	entry[0].finalDest = "CC";
	entry[0].destNode = "0";
	entry[0].weigth = 0;
	std::map<std::string, vector<EntryData>> rTable;
	//rTable["A"] = entry();
	
	for(int i=0; i<nbNode;i++)
	{
		for (int j=0;j<entry.size();j++)
		{

			char destNode[10];
	    		sprintf(destNode, "%d",j);
			entry[0].finalDest = "CC";
			entry[0].destNode = destNode;
			entry[0].weigth = j;		
		}	

}


struct Tree
{
	int timestamp;
	int nbNode;
	std::map<std::string, vector<EntryData>> Route;
}


#endif 
