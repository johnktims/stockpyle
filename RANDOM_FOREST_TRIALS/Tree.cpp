
#include "Tree.h"

Tree::Tree( std::string n )
{
	name = n;
	maxDepth = -1;
}



void Tree::presentTrainingData( DataMatrix dm, int ss )
{
	srand( ss );
	
	rootIndex = addNode( dm, rand(), 'T', 0 );

	std::cout << "\tmaxDepth=" << maxDepth << "\n\n";
}

int Tree::addNode( DataMatrix dm, int ss, char childType, int level )
{
	if( level > maxDepth )
		maxDepth = level;

	std::cout << "\tadding a node to tree... dm.vectorCount()=" << dm.vectorCount() << "  childType=" << childType << "  level=" << level << "\n" << std::flush;

	if( dm.vectorCount() < 10 )
	{
		std::cout << "\t\t{";
		for(int ii = 0; ii < dm.vectorCount(); ii++)
		{
			std::cout << dm.getFeatureVector(ii).getLabel() << " ";
		}
		std::cout << "};\n";
	}

	int ret = allNodes.size();

	Node nn;
	allNodes.push_back( nn );

	TrainingReturnType trt_ret = allNodes[ret].presentTrainingData( dm, ss );

	if( trt_ret.leafBool )
	{
		//std::cout << "\t\tLEAF\n" << std::flush;
		leftChildren.push_back(-1);
		rightChildren.push_back(-1);
		isLeaf.push_back(true);
	}

	else
	{
		//std::cout << "\t\t(int)(trt_ret.leftIndexes).size()=" << (int)(trt_ret.leftIndexes).size() << "\n" << std::flush;
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


		int lci = addNode( dm_left, rand(), 'L', level+1 );
		int rci = addNode( dm_right, rand(), 'R', level+1 );

		leftChildren.push_back( lci );
		rightChildren.push_back( rci );
		isLeaf.push_back(false);

		//std::cout << "\t\t{" << lci << "," << rci << "\n" << std::flush;
	}

	return ret;
}


ClassifyReturnType Tree::classifyVector( FeatureVector fv )
{
	int currentNodeIndex = rootIndex;
	while( !allNodes[currentNodeIndex].isLeaf() )
	{
		if( allNodes[currentNodeIndex].goLeft(fv) )
			currentNodeIndex = leftChildren[currentNodeIndex];

		else
			currentNodeIndex = rightChildren[currentNodeIndex];
	}//while not at a leaf...

	return allNodes[currentNodeIndex].classifyVector(fv);
}




