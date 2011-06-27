
#include "Node.h"


Node::Node()
{
	// just crappy defaults to mitigate seg fault possibility... 
	//but still erronious (sp?)
	nrt.leafBool = true;
	nrt.goToLeftChild = true;
	//nrt.partition
}


TrainingReturnType Node::presentTrainingData( DataMatrix dm, int ss )
{
	TrainingReturnType trt;
	(trt.partition).clear();

	if( isPure( dm ) )
	{
		trt.leafBool = true;
		//no need to build partition...
	}

	srand( ss );


	//number of features...
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
		for(int ii = 0; (!contained) && (ii < (int)testIndexes.size()); ii++)
			contained = (aTestIndex == testIndexes[ii]);
		
		if( !contained )
			testIndexes.push_back( aTestIndex );
	}//END while( testIndexes is too small )


	//go through all test indexes...
	SplitPoint bestSplit;
	bestSplit.score = -1.0;
	for(int jj = 0; jj < numberOfFeaturesToTest; jj++)
	{
		int testIndex = testIndexes[jj];	//a column index into the data matrix...
		
		SplitPoint sp = findPartition( dm, testIndex );
		if( sp.score > bestSplit.score )
		{
			bestSplit = sp;
		}
	}//END for(all test indexes)


	
	leftMean, leftVariance, rightMean, rightVariance


	TrainingReturnType trt;
	trt.leafBool = false;
	int numberOfFeatureVectors = dm.vectorCount();
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		(trt.partition).push_back(  )
}//END presentTrainingData()

bool Node::isPure( DataMatrix dm )
{
	int numberOfFeatureVectors = dm.vectorCount();

	//will be slightly off foe small values of 'numberOfFeatureVectors' (b/c N-1)
	double mean = 0.0;
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
	{
		double nextLabel = (dm.getFeatureVector(vectorIndex)).getLabel();
		sum_x += nextLabel;
		sum_x_squared += nextLabel*nextLabel;
	}//END for(vectorIndex)
	double mean = sum_x / numberOfFeatureVectors;

	//this is variance on the labels...
	double variance = (sum_x_squared/numberOfFeatureVectors) - (mean*mean);

	representativeMean = mean;
	representativeVariance = variance;

	return variance < MAXIMUM_PURE_VARIANCE;
}//END isPure()



//lots of room for optimization...
//intentionally left unoptimized for now...
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

		if( vectorIndex == 0 )
		{
			maximum_lab = newElement.l;
			minimum_lab = newElement.l;
		}

		else if( maximum_lab < newElement.l )
			maximum_lab = newElement.l;

		else if( minimum_lab > newElement.l )
			minimum_lab = newElement.l;

	}//END for(vectorIndex)

	//std::sort(cutOutColumn.begin(), cutOutColumn.end(), &Node::FeatureAndIndexSorter);
	//std::sort(cutOutColumn.begin(), cutOutColumn.end(), &FeatureAndIndexSorter);
	
	//FIX THIS!!
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
	//copy everything after the first one into the rightSide
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		rightSide.push_back( cutOutColumn[vectorIndex] );
	

	//define distribution bin size...

	int desiredNumberOfBins = sqrt(numberOfFeatureVectors);
	double binSize = (maximum_lab - minimum_lab) / desiredNumberOfBins;


	
	//try all split locations... and save best one
	//this can be optimized... no need to rebuild entire distribution each time
	double bestSplit_score = -1.0;
	int bestSplit_numberOnLeft = -1;
	while( (int)rightSide.size() > 0 )
	{
		//move the least right element from the right set to the right most in the left set
		FeatureAndIndexAndLabel mover = rightSide[0];
		rightSide.pop_front();
		leftSide.push_back( mover );



		//very much open for interpretation and change
		//score the partition scheme
		//this uses a dynamically scalled bin size to define discrete probability distribution functions
		//and then uses kullback-leibler like math for a score...
		//the distributions are built from the 'labels'
		std::vector<double> leftDistribution, rightDistribution;
		for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
		{
			leftDistribution.push_back( 0.0 );
			rightDistribution.push_back( 0.0 );
		}


		//build distributions
		for(int i = 0; i < (int)leftSide.size(); i++)					//go through left side
		{
			double nextValue = leftSide[i].l;
			int binIndex = floor( nextValue/binSize );				//right???
			leftDistribution[binIndex] += (1.0/(double)leftDistribution.size());	//for sum to 1 requirement
		}

		for(int i = 0; i < (int)rightSide.size(); i++)					//go through right side
		{
			double nextValue = rightSide[i].l;
			int binIndex = floor( nextValue/binSize );				//right???
			rightDistribution[binIndex] += (1.0/(double)rightDistribution.size());	//for sum to 1 requirement
		}


		//sign issues???
		double KB_1 = 0.0;
		double KB_2 = 0.0;
		for(int binIndex = 0; binIndex < desiredNumberOfBins; binIndex++)
		{
			double rat_1 = leftDistribution[binIndex] / rightDistribution[binIndex];
			if( rat_1 > 0.0 )
				KB_1 += leftDistribution[binIndex] * (log(rat_1) / log(2.0));

			double rat_2 = rightDistribution[binIndex] / leftDistribution[binIndex];
			if( rat_2 > 0.0 )
				KB_2 += rightDistribution[binIndex] * (log(rat_2) / log(2.0));
		}//END for(binIndex)

		double thisSplitsScore = 0.5*(KB_1 + KB_2);


		//initilize or update 'bests'
		if( (bestSplit_numberOnLeft == -1) || (thisSplitsScore > bestSplit_score) )
		{
			bestSplit_numberOnLeft = (int)leftSide.size();
			bestSplit_score = thisSplitsScore;
		}

	}//END for(splitLocation)


	SplitPoint ret;
	ret.score = bestSplit_score;
	for(int vectorIndex = 0; vectorIndex < numberOfFeatureVectors; vectorIndex++)
		(ret.partition).push_back( false );
	
	//first 'bestSplit_numberOnLeft' in 'cutOutColumn' go to the left
	for(int kk = 0; kk < bestSplit_numberOnLeft; kk++)
	{
		int indexThatGoesToLeft = cutOutColumn[kk];
		(ret.partition)[indexThatGoesToLeft] = true;
	}

	double sum_x_l = 0.0;
	double sum_x_r = 0.0;
	double sum_x_l_sq = 0.0;
	double sum_x_l_sq = 0.0;
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
	ret.lv = 

	ret.rm = sum_x_r / count_r;
	ret.rv

	return ret;
}




//bool Node::FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs)
//bool FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs)
//{
//	return lhs.f < rhs.f;
//}

