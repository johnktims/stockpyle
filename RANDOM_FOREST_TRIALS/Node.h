
#ifndef	NODE_H
#define	NODE_H	1

#include <vector>
#include <math.h>

#include "DataMatrix.h"

struct NodeReturnType
{
	bool leafBool;
	bool goToLeftChild;
	std::vector<bool> partition;
};


//very simple interface...
class Node
{
	public:
		Node();
		bool presentTrainingData( DataMatrix dm );

		NodeReturnType getNodeReturnType();


	private:
		NodeReturnType nrt;
};

#endif

