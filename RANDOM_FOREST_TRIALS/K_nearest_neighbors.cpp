//COMPILE:
//	g++ -o K_nearest_neighbors K_nearest_neighbors.cpp FeatureVector.cpp DataMatrix.cpp Node.cpp Tree.cpp Forest.cpp DataStore.cpp  -I/usr/include/mysql -I/usr/local/include/mysql++  -L/usr/local/lib -lmysqlpp -lmysqlclient -lnsl -lz -lm




#include <iostream>
#include "Forest.h"
#include <limits.h>
#include <math.h>
#include <algorithm>
#include "DataStore.h"

#define	TRAINING_RATIO		0.95
#define	NUMBER_OF_TREES		1



DataMatrix full_dm;
DataMatrix train_dm, test_dm;

std::vector<std::string> companiesOfInterest;
std::vector< std::vector<Quote> > extractedData;


struct DoubleSet
{
	std::vector<double> v;
};



bool customerSorter(DoubleSet const& lhs, DoubleSet const& rhs)
{
	return (lhs.v)[0] < (rhs.v)[0];
}




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
	companiesOfInterest.clear();
	//companiesOfInterest.push_back( (std::string)("Nasdaq:CSCO") );
	//companiesOfInterest.push_back( (std::string)("NYSE:JNPR") );
	//companiesOfInterest.push_back( (std::string)("NYSE:HPQ") );
	//companiesOfInterest.push_back( (std::string)("Nasdaq:MSFT") );
	//companiesOfInterest.push_back( (std::string)("Nasdaq:GOOG") );
	//companiesOfInterest.push_back( (std::string)("Nasdaq:YHOO") );

	companiesOfInterest.push_back( (std::string)("CSCO") );
	companiesOfInterest.push_back( (std::string)("JNPR") );
	//companiesOfInterest.push_back( (std::string)("HPQ") );
	//companiesOfInterest.push_back( (std::string)("MSFT") );
	//companiesOfInterest.push_back( (std::string)("GOOG") );
	//companiesOfInterest.push_back( (std::string)("YHOO") );
}

void buildDataMatrix(void)
{
	DataStore ds;

	Symbols symbols = ds.load_symbols();

	Quotes quotes = ds.load_quotes(WIN_90);


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
			(extractedData[symbolSelectIndex]).push_back( quotes[x] );
		}
	}//END for(x)


	int startTailSize = 3;
	int endTailSize = 3;
	for(int symbolIndex = 0; symbolIndex < (int)extractedData.size(); symbolIndex++)
	{
		for(int i = startTailSize; i < (int)extractedData[symbolIndex].size()-endTailSize; i++)
		{
			FeatureVector fv;

			//includes current day...
			//run after market close
			//std::cout << "full_dm.vectorCount(): " << full_dm.vectorCount() << "\n";
			for(int daysBack = 0; daysBack <= startTailSize; daysBack++)
			{
				Quote today = extractedData[symbolIndex][i-daysBack];

				fv.addFeature( today.close / today.open );
				fv.addFeature( today.high / today.open );
				fv.addFeature( today.low / today.open );
				fv.addFeature( (today.high - today.low) / today.open );
				fv.addFeature( today.high / today.close );
				fv.addFeature( today.low / today.close );
				fv.addFeature( (today.high - today.low) / today.close );

				//if( full_dm.vectorCount() == 346 )
				//{
				//	std::cout << "open: " << today.open << "\n";
				//	std::cout << "close: " << today.close << "\n";
				//	std::cout << "high: " << today.high << "\n";
				//	std::cout << "low: " << today.low << "\n";
				//	std::cout << "\t\t\t\t" << "(today.close / today.open): " << (today.close / today.open) << "\n";
				//	std::cout << "\t\t\t\t" << "(today.high / today.open): " << (today.high / today.open) << "\n";
				//	std::cout << "\t\t\t\t" << "(today.low / today.open): " << (today.low / today.open) << "\n";
				//}
			}

			//assume a market buy at the next day's open price
			//label with the average percentage up of the middle values			
			//double label = 0.0;
			//int pointsCounted = 0;
			//double nextDayOpen = extractedData[symbolIndex][i+1].open;	//the earliest you can buy in
			//for(int daysAhead = 1; daysAhead <= endTailSize; daysAhead++)
			//{
			//	Quote today = extractedData[symbolIndex][i+daysAhead];
			//	//double gain = today.low / extractedData[symbolIndex][i+1].open;
			//	double mid = today.low + ((today.high - today.low)/2.0);
			//	//label += ((mid - nextDayOpen)/nextDayOpen);	//percentage up from buy in
			//	label += (mid/nextDayOpen) - 1.0;	//percentage up from buy in
			//	pointsCounted++;
			//}
			//label /= pointsCounted;
			//fv.setLabel( label );

			Quote tomorrow = extractedData[symbolIndex][i+1];
			double label = (tomorrow.close - tomorrow.open) / tomorrow.open;
			//double label = tomorrow.high / tomorrow.open;
			fv.setLabel( label );


			//if( full_dm.vectorCount() == 346 )
			//{
			//	std::cout << "####\n";
			//	fv.printVector();
			//	std::cout << "\n\n";
			//}

			full_dm.addFeatureVector( fv );
		}

	}//END for(symbolIndex)

}



double distanceFunction( FeatureVector f1, FeatureVector f2 )
{
	if( f1.featureCount() != f2.featureCount() )
	{
		std::cout << "how about vectors from the same space, numb nuts?!?!\n\n";
		return -1.0;
	}

	double ret = 0.0;
	double l = 2.0;
	for(int k = 0; k < f1.featureCount(); k++)
	{
		double dif = f1.getFeature(k) - f2.getFeature(k);
		ret += pow( dif, l );
	}//END for(k)

	ret = pow(ret, (1.0/l));
	return ret;
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
	std::vector<int> trainingIndexesBackIntoFull;
	std::vector<int> testingIndexesBackIntoFull;
	for(int vectorIndex = 0; vectorIndex < numberOfVectors; vectorIndex++)
	{
		double randVal = randomDouble(0.0, 1.0);

		if( randVal < TRAINING_RATIO )
		{
			train_dm.addFeatureVector( full_dm.getFeatureVector(vectorIndex) );
			trainingIndexesBackIntoFull.push_back( vectorIndex );
		}

		else
		{
			test_dm.addFeatureVector( full_dm.getFeatureVector(vectorIndex) );
			testingIndexesBackIntoFull.push_back( vectorIndex );
		}

		//std::cout << vectorIndex << "\n";
		//(full_dm.getFeatureVector(vectorIndex)).printVector();
		//std::cout << "\n\n";
	}
	int numberOfTrainingVectors = train_dm.vectorCount();
	int numberOfTestingVectors = test_dm.vectorCount();





	int dimensionality = train_dm.getDimensionality();
	FeatureVector meanVector, expectedSquare, varianceVector;
	for(int di = 0; di < dimensionality; di++)
	{
		meanVector.addFeature( 0.0 );
		expectedSquare.addFeature( 0.0 );
		//varianceVector.addFeature( 0.0 );
	}//END for(di)


	for(int trainingIndex = 0; trainingIndex < numberOfTrainingVectors; trainingIndex++)
	{
		FeatureVector trainingVector = train_dm.getFeatureVector( trainingIndex );

		for(int di = 0; di < dimensionality; di++)
		{
			double v = trainingVector.getFeature(di);
			//meanVector.incrementFeature( di, (v / numberOfTrainingVectors) );
			//expectedSquare.incrementFeature( di, ((v*v) / numberOfTrainingVectors) );
			meanVector.incrementFeature( di, v );
			expectedSquare.incrementFeature( di, v*v );
		}
	}//END for(trainingIndex)

	for(int di = 0; di < dimensionality; di++)
	{
		meanVector.multiplyFeature( di, (1.0/numberOfTrainingVectors) );
		expectedSquare.multiplyFeature( di, (1.0/numberOfTrainingVectors) );

		double e_x2 = expectedSquare.getFeature(di);
		double ex_2 = meanVector.getFeature(di);
		varianceVector.addFeature( e_x2 - (ex_2*ex_2) );
	}//END for di




	/*
	std::vector<DoubleSet> results;
	for(int testingIndex = 0; testingIndex < numberOfTestingVectors; testingIndex++)
	{
		FeatureVector testVector = test_dm.getFeatureVector( testingIndex );

		double closestDistance = -1.0;
		FeatureVector closestVector;
		for(int trainingIndex = 0; trainingIndex < numberOfTrainingVectors; trainingIndex++)
		{
			double dd = distanceFunction( train_dm.getFeatureVector(trainingIndex), testVector );

			if( (trainingIndex == 0) || (dd < closestDistance) )
			{
				closestDistance = dd;
				closestVector = train_dm.getFeatureVector(trainingIndex);
			}
		}//END for(trainingIndex)

		std::cout << "closestDistance: " << closestDistance << "\n";
		std::cout << "actual label: " << testVector.getLabel() << "\n";
		std::cout << "predicted label: " << closestVector.getLabel() << "\n";
		std::cout << "\n\n";

		DoubleSet ds;
		(ds.v).push_back( closestDistance );
		(ds.v).push_back( testVector.getLabel() );
		(ds.v).push_back( closestVector.getLabel() );

		results.push_back( ds );

	}//END for(testingIndex)


	std::sort(results.begin(), results.end(), &customerSorter);
	for(int ii = 0; ii < (int)results.size(); ii++)
	{
		std::cout << (results[ii]).v[0] << "\t";
		std::cout << (results[ii]).v[1] << "\t";
		std::cout << (results[ii]).v[2] << "\t";
		std::cout << "\n\n";
	}*/


	for(int testingIndex = 0; testingIndex < numberOfTestingVectors; testingIndex++)
	{
		FeatureVector testVector = test_dm.getFeatureVector( testingIndex );

		std::vector<DoubleSet> aSet;
		for(int trainingIndex = 0; trainingIndex < numberOfTrainingVectors; trainingIndex++)
		{
			FeatureVector trainVector = train_dm.getFeatureVector(trainingIndex);

			double distance = 0.0;
			for(int di = 0; di < dimensionality; di++)
			{
				double meanVal = meanVector.getFeature(di);
				double stDevVal = sqrt( varianceVector.getFeature(di) );


				//could make this arithmetic simplified...
				double v1 = testVector.getFeature(di);
				v1 = (v1 - meanVal) / stDevVal;
				double v2 = trainVector.getFeature(di);
				v2 = (v2 - meanVal) / stDevVal;

				double diff = v1 - v2;
				diff *= diff;
				distance += diff;
			}//END for(di)

			//std::cout << testingIndexesBackIntoFull[testingIndex] << ": ";
			//trainVector.printVector();
			//std::cout << "\n";
			//std::cout << trainingIndexesBackIntoFull[trainingIndex] << ": ";
			//testVector.printVector();
			//std::cout << "\n";
			//std::cout << "distance: " << distance << "\n\n";


			DoubleSet structDS;
			(structDS.v).push_back( distance );
			(structDS.v).push_back( trainVector.getLabel() );
			aSet.push_back( structDS );
		}//END trainingIndex

		std::sort( aSet.begin(), aSet.end(), &customerSorter );




		//could make number of neighbors considdered dynamic, with respect to:
		//	max and min distances included
		//	variance among distances
		//	variance among associated labels
		
		std::cout << "actual label: " << testVector.getLabel() << "\n";

		int numberOfNeighbors = 5;
		double summedDistance = 0.0;
		double maxDistance = -1.0;
		for(int i = 0; i < numberOfNeighbors; i++)
		{
			summedDistance += aSet[i].v[0];

			if( aSet[i].v[0] > maxDistance )
				maxDistance = aSet[i].v[0];
		}

		double averagedLabel = 0.0;
		for(int i = 0; i < numberOfNeighbors; i++)
		{
			double dist = aSet[i].v[0];
			double weight = (1.0 - (dist/summedDistance)) / (numberOfNeighbors-1.0);
			double lab = aSet[i].v[1];

			std::cout << "\tpredicted: (" << dist << "," << weight << ") " << lab << "\n";
			averagedLabel += weight * lab;
		}

		std::cout << "\tCONFIDENCE: " << summedDistance << " " << maxDistance << "\n";
		std::cout << "\tAVERAGED: " << averagedLabel << "\n";

		std::cout << "\n\n\n";

	}//END for(testingIndex)


}//END main()



