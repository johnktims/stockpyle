
#include "Tree.h"

Tree::Tree( std::string n )
{
	name = n;
}



void Tree::presentTrainingData( DataMatrix dm, int ss )
{
	srand( ss );
	
	rootIndex = addNode( dm, rand() );
}

int Tree::addNode( DataMatrix dm, int ss )
{
	std::cout << "\tadding a node to tree... dm.vectorCount()=" << dm.vectorCount() << "\n" << std::flush;
	int ret = allNodes.size();

	Node nn;
	allNodes.push_back( nn );

	TrainingReturnType trt_ret = allNodes[ret].presentTrainingData( dm, ss );

	if( trt_ret.leafBool )
	{
		leftChildren.push_back(-1);
		rightChildren.push_back(-1);
		isLeaf.push_back(true);
		std::cout << "\t\tLEAF\n" << std::flush;
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

		std::cout << "\t\t{" << lci << "," << rci << "\n" << std::flush;
	}

	return ret;
}



