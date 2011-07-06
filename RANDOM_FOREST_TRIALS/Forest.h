
#ifndef	FOREST_H
#define	FOREST_H	1

#include <iostream>
#include <vector>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <algorithm>


#include "Tree.h"


#define	DEFAULT_TREE_COUNT	50

#define	USE_BOOTSTRAPPING	1
#define	BOOTSTRAP_RATIO		(0.6)
#define	USE_REPLACEMENT		1


class Forest
{
	public:
		Forest();
		Forest( int tc );
		void presentTrainingData( DataMatrix dm, int ss );


	private:
		int treeCount;
		std::vector<Tree> allTrees;

};



#endif

