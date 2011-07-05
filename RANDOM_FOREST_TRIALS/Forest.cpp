
#include "Forest.h"

Forest::Forest()
{
}


void Forest::presentTrainingData( DataMatrix dm_passed, int ss )
{
	srand( ss );

	for(int treeIndex = 0; treeIndex < TREE_COUNT; treeIndex++)
	{
		std::cout << "building data matrix for treeIndex=" << treeIndex << "\n" << std::flush;

		////////////////////////////////////////////////////////////////////////////////
		DataMatrix dm_toUse;
		std::vector<int> indexesToAdd;

		#if USE_BOOTSTRAPPING == 0
		//dm_toUse = dm;	//copy correctly??? or do i need to add them all??
		for(int exampleIndex = 0; exampleIndex < dm.vectorCount(); exampleIndex++)
		{
			//dm_toUse.addFeatureVector( dm.getFeatureVector(exampleIndex) );
			indexesToAdd.push_back( exampleIndex );
		}
		#endif

		#if USE_BOOTSTRAPPING == 1
		int numberToChooseFrom = dm_passed.vectorCount();
		int desiredNumberOfVectors = floor(numberToChooseFrom*BOOTSTRAP_RATIO);

		while( indexesToAdd.size() < desiredNumberOfVectors )
		{
			int indexToAdd = rand() % numberToChooseFrom;

			bool useThisIndex = true;
			#if USE_REPLACEMENT == 0
			for(int iii = 0; useThisIndex && (iii < (int)indexesToAdd.size()); iii++)
				useThisIndex = (indexToAdd == indexesToAdd[iii]);
			#endif

			if( useThisIndex )
				indexesToAdd.push_back( indexToAdd );
		}
		#endif


		for(int iii = 0; iii < (int)indexesToAdd.size(); iii++)
			dm_toUse.addFeatureVector( dm_passed.getFeatureVector( indexesToAdd[iii] ) );
		////////////////////////////////////////////////////////////////////////////////


		std::stringstream treeName(std::stringstream::in | std::stringstream::out);
		treeName << "TREE_" << treeIndex;
		Tree tt( treeName.str() );

		std::cout << "TRAINING treeIndex=" << treeIndex << "\n" << std::flush;
		allTrees.push_back( tt );
		allTrees[allTrees.size()-1].presentTrainingData( dm_toUse, rand() );


	}//END for(treeIndex)
}



