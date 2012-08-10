/*
 * ImputateInputReader.h
 *
 *  Created on: Jul 28, 2012
 *      Author: Sushil Mittal
 */

#ifndef IMPUTEINPUTREADER_H_
#define IMPUTEINPUTREADER_H_

#include "InputReader.h"

class ImputeInputReader : public InputReader {
public:
	ImputeInputReader();
	virtual ~ImputeInputReader();

	virtual void readFile(const char* fileName);
	vector<string> getColumnTypesToImpute();
	vector<int> getnMissingPerColumn();
	void setupDataForImputation(int col,vector<real>& weights);
	template <class T>
	void reindexVector(vector<T>& vec, vector<int> ind);
	void resetData();
	void getSampleMeanVariance(int col, real* Xmean, real* Xvar);
	real* getDataRow(int row, real* x);
private:
	vector<string> columnType;
	vector<int> nMissingPerColumn;
	vector<int> colIndices;
	vector<real> y_;
	vector<int_vector*> entriesAbsent;
	vector<int_vector*> entriesPresent;

	int nPatients_;
	int nCols_;
	int nRows_;
};

#endif /* IMPUTEINPUTREADER_H_ */
