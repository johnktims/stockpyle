
#ifndef	FOREST_H
#define	FOREST_H	1

#include <iostream>
#include <vector>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <algorithm>


#include "Tree.h"


#define	TREE_COUNT	100

#define	USE_BOOTSTRAPPING	1
#define	BOOTSTRAP_RATIO		(0.6)
#define	USE_REPLACEMENT		1


class Forest
{
	public:
		Forest();
		void presentTrainingData( DataMatrix dm, int ss );


	private:
		std::vector<Tree> allTrees;

};



#endif

