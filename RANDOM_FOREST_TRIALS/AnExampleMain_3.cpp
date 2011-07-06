
//tests ability to predict polynomial functions

//COMPILE:
//	g++ -o AnExampleMain_2 AnExampleMain_2.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp


#include "Forest.h"
#include <limits.h>


#define	NUMBER_OF_FEATURES		10
#define	NUMBER_OF_FEATURE_VECTORS	1000
#define	NUMBER_OF_TREES			1


DataMatrix dm;



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
		//feature_coefficients.push_back( randomDouble(-3, +4) );
		feature_coefficients.push_back( 1.0 );
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

		dm.addFeatureVector( fv );
	}//END for(vectorIndex)
}



int main( int argc, char* argv[] )
{
	srand( 0 );

	buildDataMatrix();


	Forest fst( NUMBER_OF_TREES );
	fst.presentTrainingData( dm, rand() );

	return 0;
}



