
#ifndef	DATA_MATRIX_H
#define	DATA_MATRIX_H	1

#include "FeatureVector.h"

class DataMatrix
{
	public:
		DataMatrix();

		void addFeatureMatrix( FeatureVector fv );
		FeatureVector getFeatureVector( int index );

		int vectorCount();
		int getDimensionality();
		
	private:
		std::vector<FeatureVector> matrix;
};

#endif



