
//tests ability to predict polynomial functions

//COMPILE:
//	g++ -o AnExampleMain_2 AnExampleMain_2.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp


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



void buildStatistics()
{

}





void buildDataMatrix()
{

}



int main( int argc, char* argv[] )
{
	srand( 0 );

	std::cout << "nums=[";
	for(int i = 0; i < 10000; i++)
		std::cout << randomNormal( 3, 4 ) << " ";
	std::cout << "];\n\n";



	/*buildDataMatrix();

	Forest fst( NUMBER_OF_TREES );
	fst.presentTrainingData( dm, rand() );*/

	return 0;
}



