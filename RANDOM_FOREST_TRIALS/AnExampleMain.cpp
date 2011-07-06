
//just to put the package through its paces and see if everything runs
//no real data... not even simulated real data

//COMPILE:
//	g++ -o AnExampleMain AnExampleMain.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp

#include "Forest.h"
#include <limits.h>


#define	NUMBER_OF_FEATURES		10
#define	NUMBER_OF_FEATURE_VECTORS	1000
#define	NUMBER_OF_TREES			1

int main( int argc, char* argv[] )
{
	srand( 0 );

	DataMatrix dm;
	for(int vectorIndex = 0; vectorIndex < NUMBER_OF_FEATURE_VECTORS; vectorIndex++)
	{
		double label = (2*(rand() / (double)INT_MAX)) - 1;

		FeatureVector fv( label );
		for(int featureIndex = 0; featureIndex < NUMBER_OF_FEATURES; featureIndex++)
		{
			//fv.addFeature( rand() );
			fv.addFeature( rand() / (double)INT_MAX );
		}//END for(featureIndex)

		dm.addFeatureVector( fv );
	}//END for(vectorIndex)

	Forest fst( NUMBER_OF_TREES );
	fst.presentTrainingData( dm, rand() );

	return 0;
}



