
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

std::vector<std::string> companiesOfInterest;
std::vector< std::vector<Quote> > extractedData;



//uniform ranged doubles...
double randomDouble( double min, double max )
{
	double ret = rand() / (double)INT_MAX;	//[0,1]
	ret *= (max - min);			//[0,max-min]
	ret += min;				//[min, max]
	return ret;
}

void buildCompanyVector()
{
	companiesOfInterest.push_back( (std::string)("Nasdaq:CSCO") );
	companiesOfInterest.push_back( (std::string)("NYSE:JNPR") );
	companiesOfInterest.push_back( (std::string)("NYSE:HPQ") );
	companiesOfInterest.push_back( (std::string)("Nasdaq:MSFT") );
	companiesOfInterest.push_back( (std::string)("Nasdaq:GOOG") );
	companiesOfInterest.push_back( (std::string)("Nasdaq:YHOO") );
}

void buildDataMatrix(void)
{
	DataStore ds;

	Symbols symbols = ds.load_symbols();

	Quotes quotes = ds.load_quotes(WIN_90);

	/*int x;
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
	}*/


	std::vector<Quote> dummy;
	for(int i = 0; i < (int)companiesOfInterest.size(); i++)
		extractedData.push_back( dummy );


	//build extracted data set
	for(int x = 1; x < quotes.size(); x++)
	{
		//find if and where this symbol will belong in "extractedData"
		int symbolSelectIndex = -1;
		for( int symbolTestIndex = 0; (symbolTestIndex < (int)companiesOfInterest.size()) && (symbolSelectIndex == -1); symbolTestIndex++ )
		{
			if( symbols[quotes[x].symbol_id].symbol == companiesOfInterest[symbolTestIndex] )
				symbolSelectIndex = symbolTestIndex;
		}

		if( symbolSelectIndex != -1 )
		{
			//std::cout << quotes[x].high << "\n" << std::flush;
			(extractedData[symbolSelectIndex]).push_back( quotes[x] );
		}
	}//END for(x)


	int startTailSize = 3;
	int endTailSize = 5;
	for(int symbolIndex = 0; symbolIndex < (int)extractedData.size(); symbolIndex++)
	{
		for(int i = startTailSize; i < (int)extractedData[symbolIndex].size()-endTailSize; i++)
		{
			//std::cout << symbols[ extractedData[symbolIndex][i].symbol_id ].symbol << " ";

			FeatureVector fv;

			//includes current day...
			//run after market close
			for(int daysBack = 0; daysBack < startTailSize; daysBack++)
			{
				Quote today = extractedData[symbolIndex][i-daysBack];

				fv.addFeature( today.close / today.open );
				fv.addFeature( today.high / today.open );
				fv.addFeature( today.low / today.open );
				fv.addFeature( (today.high - today.low) / today.open );
				fv.addFeature( today.high / today.close );
				fv.addFeature( today.low / today.close );
				fv.addFeature( (today.high - today.low) / today.close );
			}

			//assume a market buy at the next day's open price
			//double minGain = 0.0;
			double averageMidPoint = 0.0;
			int pointsCointed = 0;
			for(int daysAhead = 1; daysAhead <= endTailSize; daysAhead++)
			{
				Quote today = extractedData[symbolIndex][i+daysAhead];
				//double gain = today.low / extractedData[symbolIndex][i+1].open;
				double mid = today.low + (today.high - today.low);
				averageMidPoint += mid;
				pointsCointed++;
			}
			averageMidPoint /= pointsCointed;

			fv.setLabel( averageMidPoint );
		}
		std::cout << "\n\n";
	}//END for(symbolIndex)

}


int main(int argc, char* argv[] )
{
	srand( 0 );

	buildCompanyVector();

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



