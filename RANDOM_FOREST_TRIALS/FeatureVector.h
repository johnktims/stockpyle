
#ifndef	FEATURE_VECTOR_H
#define	FEATURE_VECTOR_H	1

#include <vector>
#include <iostream>

#define	DEFAULT_LABEL	0.0

class FeatureVector
{
	public:
		FeatureVector();
		FeatureVector( double l );

		void setLabel( double l );
		double getLabel();

		void addFeature( double value );
		double getFeature( int index );

		void incrementFeature( int index, double value );
		void multiplyFeature( int index, double value );

		int featureCount();

		void printVector();


	private:
		//std::vector<Feature> fv;
		std::vector<double> fv;		//could be more complex
		double label;			//could be more complex
};


#endif

