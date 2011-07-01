
#ifndef	NODE_H
#define	NODE_H	1

#include <vector>
#include <deque>
#include <math.h>
#include <stdlib.h>
#include <algorithm>

#include "DataMatrix.h"


//variance on labels
#define	MAXIMUM_PURE_VARIANCE	0.001



struct TrainingReturnType
{
	bool leafBool;
	//std::vector<bool> partition;
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


		double representativeMean, representativeVariance;	//at this node...



		//double leftMean, leftVariance, rightMean, rightVariance;
		//bool FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs);
};

#endif

