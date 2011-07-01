
#include "Tree.h"

Tree::Tree( std::string n )
{
	name = n;
}


#define	USE_BOOTSTRAPPING	1
#define	BOOTSTRAP_RATIO		(0.6)
#define	USE_REPLACEMENT		1
TrainingReturnType Tree::presentTrainingData( DataMatrix dm_passed, int ss )
{
	srand( ss );


	DataMatrix dm_toUse;
	#if USE_BOOTSTRAPPING == 0
	dm_toUse = dm_passed;	//copy correctly???
	#endif

	#if USE_BOOTSTRAPPING == 0
	int numberToChooseFrom = dm_passed.vectorCount();
	int desiredNumberOfVectors = floor(numberToChooseFrom*BOOTSTRAP_RATIO);

	std::vector<int> indexesAddedAlready;
	while( dm_toUse.vectorCount() < desiredNumberOfVectors )
	{
		int indexToAdd = rand() % numberToChooseFrom;

		bool useThisIndex = true;
		#if USE_REPLACEMENT == 0
		for(int iii = 0; useThisIndex && (iii < (int)indexesAddedAlready.size()); iii++)
			useThisIndex = (indexToAdd == indexesAddedAlready[iii]);
		#endif

		if( useThisIndex )
			indexesAddedAlready.push_back( indexToAdd );
	}

	for(int iii = 0; useThisIndex && (iii < (int)indexesAddedAlready.size()); iii++)
	{
		dm_toUse.addFeatureVector( dm_passed.getFeatureVector( indexesAddedAlready[iii] ) );
	}
	#endif

	
	rootIndex = addNode( dm_toUse, rand() );
}

int Tree::addNode( DataMatrix dm, int ss )
{
	int ret = allNodes.size();

	Node nn;
	allNodes.push_back( nn );

	TrainingReturnType trt_ret = allNodes[ret].presentTrainingData( dm, ss );

	if( trt_ret.leafBool )
	{
		leftChildren.push_back(-1);
		rightChildren.push_back(-1);
		isLeaf.push_back(true);
	}

	else
	{
		DataMatrix dm_left;
		for(int i = 0; i < (int)(trt_ret.leftIndexes).size(); i++)
		{
			int ind = (trt_ret.leftIndexes)[i];
			dm_left.addFeatureVector( dm.getFeatureVector(ind) );
		}

		DataMatrix dm_right;
		for(int i = 0; i < (int)(trt_ret.rightIndexes).size(); i++)
		{
			int ind = (trt_ret.rightIndexes)[i];
			dm_right.addFeatureVector( dm.getFeatureVector(ind) );
		}


		int lci = addNode( dm_left, rand() );
		int rci = addNode( dm_right, rand() );

		leftChildren.push_back( lci );
		rightChildren.push_back( rci );
		isLeaf.push_back(false);
	}

	return ret;
}



