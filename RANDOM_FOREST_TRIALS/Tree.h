
#ifndef	TREE_H
#define	TREE_H	1

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <algorithm>


#include "Node.h"





class Tree
{
	public:
		Tree( std::string n );
		void presentTrainingData( DataMatrix dm, int ss );


	private:
		std::string name;
		int rootIndex;
		std::vector<Node> allNodes;
		std::vector<int> leftChildren;
		std::vector<int> rightChildren;
		std::vector<bool> isLeaf;


		int addNode( DataMatrix dm, int ss );
};



#endif


