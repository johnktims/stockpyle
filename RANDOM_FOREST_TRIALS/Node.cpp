
#include "Node.h"


Node::Node()
{
	// just crappy defaults to mitigate seg fault possibility... 
	//but still erronious (sp?)
	nrt.leafBool = true;
	nrt.goToLeftChild = true;
	//nrt.partition
}


bool Node::presentTrainingData( DataMatrix dm )
{
	int dimensionality = dm.getDimensionality();

	//build a set of unique test indexes...
	std::vector<int> testIndexes;
	int numberOfFeaturesToTest = sqrt(dimensionality);
	while( (int)testIndexes.size() < numberOfFeaturesToTest )
	{
		//a possible test index
		int aTestIndex = rand() % dimensionality;

		//see if it is alreay contained...
		bool contained = false;
		for(int ii = 0; (!contained_) && (ii < (int)testIndexes.size()); ii++)
			contained = (aTestIndex == testIndexes[ii]);
		
		if( !contained )
			testIndexes.push_back( aTestIndex );
	}//END while( testIndexes is too small )


	//go through all test indexes...
	for(int jj = 0; jj < numberOfFeaturesToTest; jj++)
	{
		int testIndex = testIndexes[jj];	//a column index into the data matrix...
		
		
	}//END for(all test indexes)

	return true;
}

NodeReturnType Node::getNodeReturnType()
{
	return nrt;
}



