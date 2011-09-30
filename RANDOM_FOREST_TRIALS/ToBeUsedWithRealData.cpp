
//COMPILE:
//	g++ -o ToBeUsedWithRealData ToBeUsedWithRealData.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp


#include <iostream>
#include "Forest.h"
#include <limits.h>
#include "DataStore.h"

#define	TRAINING_RATIO		0.95
#define	NUMBER_OF_TREES		1



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

void buildDataMatrix(void)
{
    DataStore ds;

    Symbols symbols = ds.load_symbols();

    /*
    int x;
    for(x = 0; x < symbols.size(); ++x)
    {
        std::cout << symbols[x].symbol << std::endl;
    }
    */

    Quotes quotes = ds.load_quotes(WIN_30);

    int x;
    for(x = 1; x < quotes.size(); ++x)
    {
        if(symbols[quotes[x].symbol_id].sector == "Finance")
        {
            FeatureVector fv;
            fv.addFeature(quotes[x].open);
            fv.addFeature(quotes[x].high);
            fv.addFeature(quotes[x].low);
            fv.addFeature(quotes[x].close);
            fv.addFeature(quotes[x].volume);

            fv.setLabel(randomDouble(-5, 5));

            full_dm.addFeatureVector(fv);
        }
    }

}


int main(int argc, char* argv[] )
{
	srand( 0 );

	//build full data matrix, or read it in from database/python/csv file...
	buildDataMatrix();


	int numberOfVectors = full_dm.vectorCount();
    std::cout << "DataMatrix contains " << numberOfVectors << " vectors." << std::endl;
    


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



