
#include "Node.h"


Node::Node()
{
	//(rememberedSplit.leftIndexes).clear();
	//(rememberedSplit.rightIndexes).clear();
	leafiness = false;
}


TrainingReturnType Node::presentTrainingData( DataMatrix dm, int ss )
{
	srand( ss );


	TrainingReturnType trt;
	(trt.leftIndexes).clear();
	(trt.rightIndexes).clear();

	if( isPure( dm ) )
	{
		//leafiness = LEAF_CONST;
		leafiness = true;
		trt.leafBool = true;
		//no need to build partition...

		return trt;
	}
	//leafiness = NON_LEAF_CONST;
	leafiness = false;
	trt.leafBool = false;


	//number of features...
	int dimensionality = dm.getDimensionality();

	//build a set of unique test indexes...
	std::vector<int> testIndexes;
	int numberOfFeaturesToTest = (int)sqrt((double)dimensionality);
	while( (int)testIndexes.size() < numberOfFeaturesToTest )
	{
		//a possible test index
		int aTestIndex = rand() % dimensionality;

		//see if it is alreay contained...
		bool contained = false;
		for(int ii = 0; (!contained) && (ii < (int)testIndexes.size()); ii++)
		{
			//contained |= (aTestIndex == testIndexes[ii]);	//or equal faster?
			contained = (aTestIndex == testIndexes[ii]);
		}

		if( !contained )
			testIndexes.push_back( aTestIndex );
	}//END while( testIndexes is too small )


	//go through all test indexes...
	SplitPoint bestSplit;
	for(int jj = 0; jj < numberOfFeaturesToTest; jj++)
	{
		int testIndex = testIndexes[jj];	//a column index into the data matrix...
		
		SplitPoint sp = findPartition( dm, testIndex );
		if( (jj == 0 ) || (sp.score > bestSplit.score) )
			bestSplit = sp;			//everything copied properly???
		
	}//END for(all test indexes)

	//if( dm.vectorCount() < 10 )
	//{
	//	std::cout << "\t\t\tLEFT:  [";
	//	for(int i = 0; i < (int)bestSplit.leftIndexes.size(); i++)
	//		std::cout << bestSplit.leftIndexes[i] << " ";
	//	std::cout << "];\n";
	//
	//	std::cout << "\t\t\tRIGHT: [";
	//	for(int i = 0; i < (int)bestSplit.rightIndexes.size(); i++)
	//		std::cout << bestSplit.rightIndexes[i] << " ";
	//	std::cout << "];\n";
	//}



	//TrainingReturnType trt;	
	for(int vectorIndex = 0; vectorIndex < (int)(bestSplit.leftIndexes).size(); vectorIndex++)
			(trt.leftIndexes).push_back( (bestSplit.leftIndexes)[vectorIndex] );

	for(int vectorIndex = 0; vectorIndex < (int)(bestSplit.rightIndexes).size(); vectorIndex++)
			(trt.rightIndexes).push_back( (bestSplit.rightIndexes)[vectorIndex] );



	//too much room to save the partitions... 
	//and not really useful for future classification...
	//(that is if the saved items {mean and variance} are actually representative of the items here
	(bestSplit.leftIndexes).clear();
	(bestSplit.rightIndexes).clear();
	rememberedSplit = bestSplit;


	return trt;
	
}//END presentTrainingData()

bool Node::isPure( DataMatrix dm )
{
	int numberOfFeatureVectors = dm.vectorCount();


	//if( numberOfFeatureVectors <= MINIMUM_NUMBER_OF_VECTORS )
	//	return true;

	//will be slightly off foe small values of 'numberOfFeatureVectors' (b/c N-1)
	double sum_x = 0.0;
	double sum_x_squared = 0.0;
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		double nextLabel = (dm.getFeatureVector(vectorIndex)).getLabel();
		sum_x += nextLabel;
		sum_x_squared += nextLabel*nextLabel;
	}//END for(vectorIndex)
	double mean = sum_x / numberOfFeatureVectors;


	//this is variance on the labels...
	double variance = (sum_x_squared/numberOfFeatureVectors) - (mean*mean);


	//if at this node... the items previously here had this distribution...
	//could be more complex...
	//representativeMean = mean;
	//representativeVariance = variance;
	representative.m = mean;
	representative.v = variance;

	return (variance < MAXIMUM_PURE_VARIANCE) || (numberOfFeatureVectors <= MINIMUM_NUMBER_OF_VECTORS);
}//END isPure()



//lots of room for optimization...
//intentionally left unoptimized for now...
//expects non zero span of labels presented...
SplitPoint Node::findPartition( DataMatrix dm, int featureIndex )
{
	int numberOfFeatureVectors = dm.vectorCount();


	//find minimum and maximum label values
	//as well as build 'cutOutColumn' for this feature index
	std::vector<FeatureAndIndexAndLabel> cutOutColumn;
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		FeatureAndIndexAndLabel newElement;
		newElement.i = vectorIndex;
		newElement.f = (dm.getFeatureVector(vectorIndex)).getFeature(featureIndex);
		newElement.l = (dm.getFeatureVector(vectorIndex)).getLabel();


		cutOutColumn.push_back( newElement );

	}//END for(vectorIndex)

	
	//FIX THIS!!!	???
	//sort the cut column based on the feature values
	//to order the distribution...
	bool swapped = true;
	while( swapped )
	{
		swapped = false;
		//for each i in 1 to length(A) - 1 inclusive do:
		for(int i = 1; i < (int)cutOutColumn.size(); i++)
		{
			//if A[i-1] > A[i] then
			if( cutOutColumn[i-1].f > cutOutColumn[i].f )
			{
				//swap( A[i-1], A[i] )
				FeatureAndIndexAndLabel moveElement = cutOutColumn[i-1];
				cutOutColumn[i-1] = cutOutColumn[i];
				cutOutColumn[i] = moveElement;
				
				swapped = true;
			}
		}
	}//END sort...	



	std::deque<FeatureAndIndexAndLabel> leftSide, rightSide;

	//copy everything after the first one into the 'rightSide'
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		rightSide.push_back( cutOutColumn[vectorIndex] );





	
	//try all split locations... and save best one
	//this can be optimized... no need to rebuild entire distribution each time
	double bestSplit_score = -1;
	int bestSplit_numberOnLeft = -1;
	while( (int)rightSide.size() >= 2 )	//STILL HAS 1 TO "GIVE" TO 'LEFT'
	{
		//move the least right element from the right set to the right most in the left set
		FeatureAndIndexAndLabel mover = rightSide[0];
		rightSide.pop_front();
		leftSide.push_back( mover );


		std::vector<double> labels_left;
		for(int ind = 0; ind < (int)leftSide.size(); ind++)
			labels_left.push_back( leftSide[ind].l );
		sort(labels_left.begin(), labels_left.end());

		std::vector<double> labels_right;
		for(int ind = 0; ind < (int)rightSide.size(); ind++)
			labels_right.push_back( rightSide[ind].l );
		sort(labels_right.begin(), labels_right.end());


		std::vector<double> sorted_labels;
		std::vector<bool> belongs_to_left;
		std::vector<double> ranks;


		int li = 0;	//left index
		int ri = 0;
		double le, re;
		while( (li < labels_left.size()) && (ri < labels_right.size()) )
		{
			le = labels_left[li];	//left element
			re = labels_right[ri];

			if( le < re )
			{
				sorted_labels.push_back( le );
				belongs_to_left.push_back( true );
				li++;
			}

			else
			{
				sorted_labels.push_back( re );
				belongs_to_left.push_back( false );
				ri++;
			}
		}//END while(both have stuff to give)

		//only one of these while loops should execute
		while( li < (int)labels_left.size() )
		{
			le = labels_left[li];
			sorted_labels.push_back( le );
			belongs_to_left.push_back( true );
			li++;
		}

		while( ri < (int)labels_right.size() )
		{
			le = labels_right[ri];
			sorted_labels.push_back( le );
			belongs_to_left.push_back( false );
			ri++;
		}



		for(int ind = 0; ind < (int)sorted_labels.size(); ind++)
			ranks.push_back( ind+1 );

		//break them ties...
		int si = 0;	//sorted index
		while( si < (int)ranks.size() )
		{
			int start_si = si;
			double rankSum = ranks[si];
			int countInvolved = 1;

			while( (si+1 < (int)ranks.size()) && (sorted_labels[si] == sorted_labels[si+1]) )
			{
				rankSum += ranks[si+1];
				si++;
				countInvolved++;
			}
			
			double avgRank = rankSum / countInvolved;
			for(int ind = start_si; ind <= si; ind++)
				ranks[ind] = avgRank;

			si++;
		}//END while( si still small )
		

		double sum_1 = 0.0;
		double sum_2 = 0.0;
		for(int ind = 0; ind < (int)ranks.size(); ind++)
		{
			if( belongs_to_left[ind] )
				sum_1 += ranks[ind];

			else
				sum_2 += ranks[ind];
		}

		double n1 = (double)labels_left.size();
		double U_1 = sum_1 - ((n1*n1 + n1) / 2);

		double n2 = (double)labels_right.size();
		double U_2 = sum_2 - ((n2*n2 + n2) / 2);

		double sc = 1.0 / fabs(U_1*U_2);

		if( sc > bestSplit_score )
		{
			bestSplit_score = sc;
			bestSplit_numberOnLeft = (int)leftSide.size();
		}


	}//END for(splitLocation)


	SplitPoint ret;
	ret.score = bestSplit_score;
	ret.featInd = featureIndex;


	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		int orig_index = cutOutColumn[vectorIndex].i;

		if( vectorIndex < bestSplit_numberOnLeft )
			(ret.leftIndexes).push_back( orig_index );

		else
			(ret.rightIndexes).push_back( orig_index );
	}//END for(vectorIndex)

	double sum_x_l = 0.0;
	double sum_x_r = 0.0;
	double sum_x_l_sq = 0.0;
	double sum_x_r_sq = 0.0;
	int count_l = 0;
	int count_r = 0;
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		double featVal = cutOutColumn[vectorIndex].f;
		if( vectorIndex < bestSplit_numberOnLeft )
		{
			sum_x_l += featVal;
			sum_x_l_sq += featVal*featVal;
			count_l++;		//should be 'bestSplit_numberOnLeft' when done
		}

		else
		{
			sum_x_r += featVal;
			sum_x_r_sq += featVal*featVal;
			count_r++;		//should be ('numberOfFeatureVectors'-'bestSplit_numberOnLeft') when done
		}
	}

	ret.featInd = featureIndex;

	ret.lm = sum_x_l / count_l;
	ret.lv = (sum_x_l_sq / count_l) - (ret.lm*ret.lm);

	ret.rm = sum_x_r / count_r;
	ret.rv = (sum_x_r_sq / count_r) - (ret.rm*ret.rm);

	return ret;
}




//bool Node::FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs)
//bool FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs)
//{
//	return lhs.f < rhs.f;
//}



bool Node::isLeaf()
{
	return leafiness;
}

//assumes that this node is NOT a leaf...
//it may seg fault otherwise...
bool Node::goLeft( FeatureVector fv )
{
	//std::cout << "\t" << rememberedSplit.featInd << " " << fv.featureCount() << " ~~~ " << leafiness << "\n";
	double fVal = fv.getFeature( rememberedSplit.featInd );

	double off_l = fVal - rememberedSplit.lm;	//dist from left mean
	double d_l = (off_l*off_l) / (2*rememberedSplit.lv);
	double pdfAmp_l = (1.0/sqrt(2*3.14*rememberedSplit.lv)) * exp(-d_l);

	double off_r = fVal - rememberedSplit.rm;	//dist from right mean
	double d_r = (off_r*off_r) / (2*rememberedSplit.rv);
	double pdfAmp_r = (1.0/sqrt(2*3.14*rememberedSplit.rv)) * exp(-d_r);

	return pdfAmp_l > pdfAmp_r;
}

ClassifyReturnType Node::classifyVector( FeatureVector fv )
{
	return representative;
}










