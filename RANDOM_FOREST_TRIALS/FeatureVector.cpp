
#include "FeatureVector.h"

FeatureVector::FeatureVector()
{
	label = DEFAULT_LABEL;
}

FeatureVector::FeatureVector( double l )
{
	label = l;
}


void FeatureVector::setLabel( double l )
{
	label = l;
}

double FeatureVector::getLabel()
{
	return label;
}


void FeatureVector::addFeature( double value )
{
	fv.push_back( value );
}

double FeatureVector::getFeature( int index )
{
	if( (index < 0) || (index >= (int)fv.size()) )
	{
		std::cout << "FeatureVector::getFeature() ~~~ INDEX OUT OF BOUNDS...\n\n" << std::flush;
		return 0.0;
	}

	else
	{
		return fv[index];
	}
}

void FeatureVector::incrementFeature( int index, double value )
{
	if( (index < 0) || (index >= (int)fv.size()) )
	{
		std::cout << "FeatureVector::incrementFeature() ~~~ INDEX OUT OF BOUNDS...\n\n" << std::flush;
	}

	else
	{
		fv[index] += value;
	}
}

void FeatureVector::multiplyFeature( int index, double value )
{
	if( (index < 0) || (index >= (int)fv.size()) )
	{
		std::cout << "FeatureVector::multiplyFeature() ~~~ INDEX OUT OF BOUNDS...\n\n" << std::flush;
	}

	else
	{
		fv[index] *= value;
	}
}

int FeatureVector::featureCount()
{
	return (int)fv.size();
}

void FeatureVector::printVector()
{
	std::cout << "{";
	for(int i = 0; i < (int)fv.size(); i++)
		std::cout << fv[i] << " ";
	std::cout << "}";
}





