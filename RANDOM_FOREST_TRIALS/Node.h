
#ifndef	NODE_H
#define	NODE_H	1

#include <vector>
#include <deque>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "DataMatrix.h"


//variance on labels
#define	MAXIMUM_PURE_VARIANCE		0.001
#define	MINIMUM_NUMBER_OF_VECTORS	3



struct TrainingReturnType
{
	bool leafBool;
	std::vector<int> leftIndexes;
	std::vector<int> rightIndexes;
};


struct SplitPoint
{
	//std::vector<bool> partition;
	std::vector<int> leftIndexes;
	std::vector<int> rightIndexes;

	double score;			//to maximize
	int featInd;


	//stat vals on the feature
	double lm, rm, lv, rv;		//means and variances on FEATURE
};

struct FeatureAndIndexAndLabel
{
	int i;
	double f;
	double l;
};




class Node
{
	public:
		Node();
		TrainingReturnType presentTrainingData( DataMatrix dm, int ss );


	private:
		SplitPoint rememberedSplit;	//partition not remembered...

		bool isPure( DataMatrix dm );

		SplitPoint findPartition( DataMatrix dm, int featureIndex );


		//at this node...
		//could be more complex...
		double representativeMean, representativeVariance;



		//double leftMean, leftVariance, rightMean, rightVariance;
		//bool FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs);
};

#endif

