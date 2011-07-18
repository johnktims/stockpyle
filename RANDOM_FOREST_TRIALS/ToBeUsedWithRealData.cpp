
//COMPILE:
//	g++ -o ToBeUsedWithRealData ToBeUsedWithRealData.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp


#include "Forest.h"
#include <limits.h>

#define	TRAINING_RATIO		0.7
#define	NUMBER_OF_TREES		1



//used to generate toy data...
//remove when using REAL data
#define	NUMBER_OF_FEATURES		10
#define	NUMBER_OF_FEATURE_VECTORS	10



DataMatrix full_dm;
DataMatrix train_dm, test_dm;





//uniform ranged doubles...
double randomDouble( double min, double max )
{
	double ret = rand() / (double)INT_MAX;	//[0,1]
	ret *= (max - min);			//[0,max-min]
	ret += min;				//[min, max]
	return ret;
}

void buildDataMatrix()
{
	std::vector<double> feature_coefficients;
	std::vector<double> feature_exponents;

	for(int featureIndex = 0; featureIndex < NUMBER_OF_FEATURES; featureIndex++)
	{
		feature_coefficients.push_back( randomDouble(-3, +4) );
		//feature_coefficients.push_back( 1.0 );
		//feature_exponents.push_back( randomDouble(0, +2) );
		feature_exponents.push_back( 1.0 );
	}


	for(int vectorIndex = 0; vectorIndex < NUMBER_OF_FEATURE_VECTORS; vectorIndex++)
	{
		double label = 0.0;

		FeatureVector fv;
		for(int featureIndex = 0; featureIndex < NUMBER_OF_FEATURES; featureIndex++)
		{
			double newFeature = randomDouble( -2, +2 );
			fv.addFeature( newFeature );
			label += feature_coefficients[featureIndex] * pow(newFeature, feature_exponents[featureIndex]);
		}//END for(featureIndex)

		fv.setLabel( label );

		full_dm.addFeatureVector( fv );
	}//END for(vectorIndex)
}



int main(int argc, char* argv[] )
{
	srand( 0 );

	//build full data matrix, or read it in from database/python/csv file...
	buildDataMatrix();











	int numberOfVectors = full_dm.vectorCount();


	//build training and testing data matricies
	for(int vectorIndex = 0; vectorIndex < numberOfVectors; vectorIndex++)
	{
		double randVal = randomDouble(0.0, 1.0);

		if( randVal < TRAINING_RATIO )
			train_dm.addFeatureVector( full_dm.getFeatureVector(vectorIndex) );

		else
			test_dm.addFeatureVector( full_dm.getFeatureVector(vectorIndex) );
	}
	int numberOfTrainingVectors = train_dm.vectorCount();
	int numberOfTestingVectors = test_dm.vectorCount();

	
	//build and train a forest
	Forest fst( NUMBER_OF_TREES );
	fst.presentTrainingData( train_dm, rand() );


	//test the fully trained forest
	for(int vectorIndex = 0; vectorIndex < numberOfTestingVectors; vectorIndex++)
	{
		fst.classifyVector( test_dm.getFeatureVector(vectorIndex) );
	}//END for(vectorIndex)

	return 0;
}


