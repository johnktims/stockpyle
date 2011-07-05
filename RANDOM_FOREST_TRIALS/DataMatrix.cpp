
#include "DataMatrix.h"

DataMatrix::DataMatrix()
{
}

void DataMatrix::addFeatureVector( FeatureVector fv )
{
	if( (vectorCount() == 0) )
		matrix.push_back( fv );

	else if(matrix[0].featureCount() == fv.featureCount())
		matrix.push_back( fv );
	
	else
		std::cout << "DataMatrix::addFeatureVector() ~~~ dimensionality error\n\n" << std::flush;
}

FeatureVector DataMatrix::getFeatureVector( int index )
{
	if( (index < 0) || (index >= (int)matrix.size()) )
	{
		std::cout << "DataMatrix::getFeatureVector() ~~~ INDEX OUT OF BOUNDS...\n\n" << std::flush;
		FeatureVector fv;
		return fv;
	}

	else
	{
		return matrix[index];
	}
}

int DataMatrix::vectorCount()
{
	return (int)matrix.size();
}

int DataMatrix::getDimensionality()
{
	if( (vectorCount() == 0) )
		return 0;

	else
		return (matrix[0]).featureCount();
}



