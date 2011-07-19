
//tests ability to predict polynomial functions

//COMPILE:
//	g++ -o AnExampleMain_3 AnExampleMain_3.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp


#include "Forest.h"
#include <limits.h>


#define	NUMBER_OF_FEATURES			10
#define	NUMBER_OF_TYPES				5
#define	NUMBER_OF_FEATURE_VECTORS_PER_TYPE	1000

#define	NUMBER_OF_TREES				1


struct FeatureStatistics
{
	std::vector<double> coefficientVector;
	std::vector<double> exponentialVector;

	std::vector<double> meanVector;
	std::vector<double> varianceVector;
};



double means[NUMBER_OF_TYPES][NUMBER_OF_FEATURES];
double variances[NUMBER_OF_TYPES][NUMBER_OF_FEATURES];
DataMatrix dm;


//uniform ranged doubles...
double randomDouble( double min, double max )
{
	double ret = rand() / (double)INT_MAX;	//[0,1]
	ret *= (max - min);			//[0,max-min]
	ret += min;				//[min, max]
	return ret;
}


double randomNormal( double m, double v )
{
	int n = 100;
	std::vector<double> y;
	double sum_y = 0.0;
	for(int i = 0; i < n; i++)
	{
		double newVal = rand();
		y.push_back( newVal );
		sum_y += newVal;
	}

	double ret = ((1.0/INT_MAX) * sqrt(12.0/n) * sum_y) - sqrt(3*n);
	ret *= sqrt(v);
	ret += m;

	return ret;
}


#define	NUMBER_OF_FEATURES			10
#define	NUMBER_OF_TYPES				5
void buildStatistics()
{
	for(int ti = 0; ti < NUMBER_OF_TYPES; ti++)
	{
		for(int fi = 0; fi < NUMBER_OF_FEATURES; fi++)
		{
			means[ti][fi] = randomDouble(-5, +5);
			variances[ti][fi] = randomDouble(1, +5);
		}
	}//END for(ti)
}





void buildDataMatrix()
{

}



int main( int argc, char* argv[] )
{
	srand( time( NULL ) );

	buildStatistics();
	buildDataMatrix();

	Forest fst( NUMBER_OF_TREES );
	fst.presentTrainingData( dm, rand() );

	return 0;
}



