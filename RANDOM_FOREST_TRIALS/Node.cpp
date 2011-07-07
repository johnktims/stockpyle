
#include "Node.h"


Node::Node()
{
	//(rememberedSplit.leftIndexes).clear();
	//(rememberedSplit.rightIndexes).clear();
}


TrainingReturnType Node::presentTrainingData( DataMatrix dm, int ss )
{
	srand( ss );


	TrainingReturnType trt;
	(trt.leftIndexes).clear();
	(trt.rightIndexes).clear();

	if( isPure( dm ) )
	{
		leafiness = true;
		trt.leafBool = true;
		//no need to build partition...

		return trt;
	}


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

	if( dm.vectorCount() < 10 )
	{
		std::cout << "\t\t\tLEFT:  [";
		for(int i = 0; i < (int)bestSplit.leftIndexes.size(); i++)
			std::cout << bestSplit.leftIndexes[i] << " ";
		std::cout << "];\n";

		std::cout << "\t\t\tRIGHT: [";
		for(int i = 0; i < (int)bestSplit.rightIndexes.size(); i++)
			std::cout << bestSplit.rightIndexes[i] << " ";
		std::cout << "];\n";
	}



	//TrainingReturnType trt;
	trt.leafBool = false;
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

	if( numberOfFeatureVectors <= MINIMUM_NUMBER_OF_VECTORS )
		return true;

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

	return variance < MAXIMUM_PURE_VARIANCE;
}//END isPure()



//lots of room for optimization...
//intentionally left unoptimized for now...
//expects non zero span of labels presented...
SplitPoint Node::findPartition( DataMatrix dm, int featureIndex )
{
	int numberOfFeatureVectors = dm.vectorCount();


	//find minimum and maximum label values
	//as well as build 'cutOutColumn' for this feature index
	double maximum_lab = 0.0;
	double minimum_lab = 0.0;
	std::vector<FeatureAndIndexAndLabel> cutOutColumn;
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		FeatureAndIndexAndLabel newElement;
		newElement.i = vectorIndex;
		newElement.f = (dm.getFeatureVector(vectorIndex)).getFeature(featureIndex);
		newElement.l = (dm.getFeatureVector(vectorIndex)).getLabel();


		cutOutColumn.push_back( newElement );


		//init max and min
		if( vectorIndex == 0 )
		{
			maximum_lab = newElement.l;
			minimum_lab = newElement.l;
		}

		//update max
		else if( maximum_lab < newElement.l )
			maximum_lab = newElement.l;

		//update min
		else if( minimum_lab > newElement.l )
			minimum_lab = newElement.l;

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
	//double left_label_sum = 0.0;
	//double right_label_sum = 0.0;

	//copy everything after the first one into the 'rightSide'
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		rightSide.push_back( cutOutColumn[vectorIndex] );
		//right_label_sum += cutOutColumn[vectorIndex].l;
	}


	/*std::cout << "allIndexes=[";
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		std::cout << cutOutColumn[vectorIndex].i << " ";
	std::cout << "];\n\n";

	std::cout << "allFeatures=[";
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		std::cout << cutOutColumn[vectorIndex].f << " ";
	std::cout << "];\n\n";

	std::cout << "allLabels=[";
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		std::cout << cutOutColumn[vectorIndex].l << " ";
	std::cout << "];\n\n";


	char aaa;
	std::cin >> aaa;*/




	//define distribution bin size...
	int desiredNumberOfBins = sqrt(numberOfFeatureVectors);
	double binSize = (maximum_lab - minimum_lab) / desiredNumberOfBins;
	
	//try all split locations... and save best one
	//this can be optimized... no need to rebuild entire distribution each time
	double bestSplit_score = -1;
	int bestSplit_numberOnLeft = -1;
	while( (int)rightSide.size() >= 2 )	//STILL HAS 1 TO GIVE TO 'LEFT'
	{
		//move the least right element from the right set to the right most in the left set
		FeatureAndIndexAndLabel mover = rightSide[0];
		rightSide.pop_front();
		leftSide.push_back( mover );

		/*right_label_sum -= mover.l;
		left_label_sum += mover.l;

		double right_label_mean = right_label_sum / rightSide.size();
		double left_label_mean = left_label_sum / leftSide.size();

		double nextScore = fabs(right_label_mean - left_label_mean);
		if(nextScore > bestSplit_score)
		{
			bestSplit_numberOnLeft = (int)leftSide.size();
			bestSplit_score = nextScore;
		}*/


		//very much open for interpretation and change
		//score the partition scheme
		//this uses a dynamically scalled bin size to define discrete probability distribution functions
		//the distributions are built from the 'labels'

		std::vector<double> leftDistribution, rightDistribution;
		for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
		{
			leftDistribution.push_back( 0.0 );
			rightDistribution.push_back( 0.0 );
		}

		//build distributions
		double l_dist_sum = 0.0;
		double r_dist_sum = 0.0;
		for(int i = 0; i < (int)leftSide.size(); i++)					//go through left side
		{
			double nextValue = leftSide[i].l;
			int binIndex = floor( (nextValue - minimum_lab)/binSize );				//right???
			leftDistribution[binIndex] += (1.0/(double)leftSide.size());	//for sum to 1 requirement
		}

		for(int i = 0; i < (int)rightSide.size(); i++)					//go through right side
		{
			double nextValue = rightSide[i].l;
			int binIndex = floor( (nextValue - minimum_lab)/binSize );				//right???
			rightDistribution[binIndex] += (1.0/(double)rightSide.size());	//for sum to 1 requirement
		}

		/*//check defined criteria
		//for all i: (P(i)>0) -> (Q(i)>0)
		bool zeroHeightRequirementSatisfied = true;
		for(int binIndex = 0; zeroHeightRequirementSatisfied && (binIndex < desiredNumberOfBins); binIndex++)
		{
			bool atLeastOneZero = (leftDistribution[binIndex]*rightDistribution[binIndex] == 0.0);
			bool bothZero = (leftDistribution[binIndex]+rightDistribution[binIndex] == 0.0);

			if( atLeastOneZero && bothZero )
				zeroHeightRequirementSatisfied = false;
		}*/

		/*printf("l_dist=[");
		for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
			printf("%2.2f  ", leftDistribution[binIndex]);
		printf("];\nr_dist=[");

		for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
			printf("%2.2f  ", rightDistribution[binIndex]);
		printf("];\n");
		printf("\n");*/




		/*if( zeroHeightRequirementSatisfied )
		{
			//sign issues???
			double KB_1 = 0.0;
			double KB_2 = 0.0;
			for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
			{
				double rat_1 = leftDistribution[binIndex] / rightDistribution[binIndex];
				if( leftDistribution[binIndex] > 0.0 )
					KB_1 += leftDistribution[binIndex] * (log(rat_1) / log(2.0));

				double rat_2 = rightDistribution[binIndex] / leftDistribution[binIndex];
				if( rat_2 > rightDistribution[binIndex] )
					KB_2 += rightDistribution[binIndex] * (log(rat_2) / log(2.0));
			}//END for(binIndex)

			double thisSplitsScore = 0.5*(KB_1 + KB_2);

			std::cout << "\t\tthisSplitsScore: " << thisSplitsScore << "\n" << std::flush;
			//initilize or update 'bests'
			if( (bestSplit_numberOnLeft == -1) || (thisSplitsScore > bestSplit_score) )
			{
				bestSplit_numberOnLeft = (int)leftSide.size();
				bestSplit_score = thisSplitsScore;
			}
		}//if( zeroHeightRequirementSatisfied )*/


		int L_value = 2;	//restricted to int... its just cleaner to talk about
		double distributionDistance = 0.0;
		for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
		{
			double dd = fabs( leftDistribution[binIndex] - rightDistribution[binIndex] );
			distributionDistance += pow(dd, L_value);
		}
		distributionDistance = pow(distributionDistance, (1.0/L_value));

		//std::cout << "~~~ " << distributionDistance << " " << (int)leftSide.size() << " ";
		//std::cout << distributionDistance << " ";

		if( (bestSplit_numberOnLeft == -1) || (distributionDistance > bestSplit_score) )
		{
			//std::cout << "***";
			bestSplit_numberOnLeft = (int)leftSide.size();
			bestSplit_score = distributionDistance;
		}
		//std::cout << "\n";

	}//END for(splitLocation)
	//std::cout << "\n\n";


	SplitPoint ret;
	ret.score = bestSplit_score;


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










