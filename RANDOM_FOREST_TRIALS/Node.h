
#ifndef	NODE_H
#define	NODE_H	1

#include <vector>
#include <deque>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "DataMatrix.h"


//purity on labels
//5*sqrt(v) = 0.5 -> v = 0.01
//(even 5 standard deviations out, the values are within 0.5 <half a percent> of mean)
#define	MAXIMUM_PURE_VARIANCE		0.01
#define	MINIMUM_NUMBER_OF_VECTORS	3


#define	LEAF_CONST	1
#define	NON_LEAF_CONST	0



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


struct ClassifyReturnType
{
	double m, v;
};




class Node
{
	public:
		Node();
		TrainingReturnType presentTrainingData( DataMatrix dm, int ss );

		int isLeaf();
		bool goLeft( FeatureVector fv );
		ClassifyReturnType classifyVector( FeatureVector fv );


	private:
		int leafiness;
		SplitPoint rememberedSplit;	//partition not remembered...

		bool isPure( DataMatrix dm );

		SplitPoint findPartition( DataMatrix dm, int featureIndex );


		//at this node...
		//could be more complex...
		//double representativeMean, representativeVariance;
		ClassifyReturnType representative;



		//double leftMean, leftVariance, rightMean, rightVariance;
		//bool FeatureAndIndexSorter(FeatureAndIndex const& lhs, FeatureAndIndex const& rhs);
};

#endif

