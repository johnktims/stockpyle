
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

	std::cout << "\tmaxDepth  = " << maxDepth << "\n";
	std::cout << "\trootIndex = " << rootIndex << "\n\n";
}

int Tree::addNode( DataMatrix dm, int ss, char childType, int level )
{
	if( level > maxDepth )
		maxDepth = level;

	//std::cout << "\tadding a node to tree... dm.vectorCount()=" << dm.vectorCount() << "  childType=" << childType << "  level=" << level << "\n" << std::flush;


	int ret = allNodes.size();	//this node's position

	Node nn;
	allNodes.push_back( nn );


	TrainingReturnType trt_ret = allNodes[ret].presentTrainingData( dm, ss );

	//assume its a leaf...
	//and ensure that the index in 'left'/'right'  belongs to THIS node!!!
	leftChildren.push_back(-1);
	rightChildren.push_back(-1);

	//std::cout << "ADD_NODE()\n";
	//std::cout << "\ttrt_ret.leafBool: " << trt_ret.leafBool << "\n";
	//std::cout << "\tposition: " << ret << "\n";
	//for(int i = 0; i < dm.vectorCount(); i++)
	//	std::cout << "\t\t" << (dm.getFeatureVector(i)).getLabel() << "\n";



	if( trt_ret.leafBool )
	{
		//std::cout << "\t\tLEAF\n" << std::flush;
		//leftChildren.push_back(-1);
		//rightChildren.push_back(-1);
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

		//leftChildren.push_back( lci );
		//rightChildren.push_back( rci );

		leftChildren[ret] = lci;
		rightChildren[ret] = rci;
		isLeaf.push_back(false);

		//std::cout << "\t\t{" << lci << "," << rci << "\n" << std::flush;
	}

	return ret;
}


ClassifyReturnType Tree::classifyVector( FeatureVector fv )
{
	//std::cout << "LEFT_CHILDREN=[";
	//for(int i = 0; i < (int)leftChildren.size(); i++)
	//	std::cout << leftChildren[i] << " ";
	//std::cout << "];\n\n";
	//
	//std::cout << "RIGHT_CHILDREN=[";
	//for(int i = 0; i < (int)rightChildren.size(); i++)
	//	std::cout << rightChildren[i] << " ";
	//std::cout << "];\n\n";



	int currentNodeIndex = rootIndex;
	//while( !allNodes[currentNodeIndex].isLeaf() )
	while( allNodes[currentNodeIndex].isLeaf() == NON_LEAF_CONST )
	{
		//std::cout << "\n\n\ncurrentNodeIndex = " << currentNodeIndex << "\n";
		//std::cout << "allNodes[currentNodeIndex].isLeaf() = " << allNodes[currentNodeIndex].isLeaf() << "\n";
		bool directedToGoLeft = allNodes[currentNodeIndex].goLeft(fv);
		//std::cout << "directedToGoLeft: " << directedToGoLeft << "\n";
		if( directedToGoLeft )
			currentNodeIndex = leftChildren[currentNodeIndex];

		else
			currentNodeIndex = rightChildren[currentNodeIndex];
	}//while not at a leaf...

	return allNodes[currentNodeIndex].classifyVector(fv);
}




