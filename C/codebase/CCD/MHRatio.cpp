/*
 * MHRatio.cpp
 *
 *  Created on: Aug 6, 2012
 *      Author: trevorshaddox
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <time.h>
#include <set>

#include <math.h>

#include "MHRatio.h"

#include <boost/random.hpp>
#include <boost/random/uniform_real.hpp>

#include <Eigen/Dense>
#include <Eigen/Cholesky>

#define Debug_TRS

namespace bsccs{

MHRatio::MHRatio(CyclicCoordinateDescent & ccd){
	storedFBetaCurrent = ccd.getLogLikelihood();
	storedPBetaCurrent = ccd.getLogPrior();

}

MHRatio::~MHRatio(){

}

double MHRatio::evaluate(Parameter * Beta, Parameter * Beta_Hat, Parameter * SigmaSquared, CyclicCoordinateDescent & ccd, boost::mt19937& rng, Eigen::MatrixXf PrecisionMatrix) {

// Get the proposed Beta values
	vector<double> * betaPossible = Beta->returnCurrentValuesPointer();

	double hastingsRatio = getHastingsRatio(Beta,Beta_Hat, PrecisionMatrix);

// Compute log Likelihood and log prior
#ifdef Debug_TRS
	int lengthIs = Beta->getSize();
	cout << "Printing Stored Beta in CCD" << endl;
	cout << "<";
	for (int i = 0; i < lengthIs; i ++) {
		cout << ccd.getBeta(i) << ", ";
	}
	cout << endl;
#endif


	ccd.resetBeta();
	ccd.setBeta(*betaPossible);


#ifdef Debug_TRS
	cout << "Printing Proposed Beta in CCD" << endl;
	cout << "<";
	for (int i = 0; i < lengthIs; i ++) {
		cout << ccd.getBeta(i) << ", ";
	}
	cout << endl;
#endif

	double fBetaPossible = ccd.getLogLikelihood();
	double pBetaPossible = ccd.getLogPrior();

// Have we changed the Beta Values?  If so, get new Log Likelihood and prior values...
	//if (!storedValuesUpToDate) {

		vector<double> * betaOldValues = Beta->returnStoredValuesPointer();
		ccd.setBeta(*betaOldValues);   // Just keep track of current logLikelihood... its a number, cache, do not recompute
	//}


	(ccd.hXI_Transpose).setUseThisStatus(true); // Testing code

// Compute the ratio for the MH step
	double ratio = exp((fBetaPossible + pBetaPossible) - (storedFBetaCurrent + storedPBetaCurrent));

// Set our alpha
	alpha = min(ratio, 1.0);
	static boost::uniform_01<boost::mt19937> zeroone(rng);

// Sample from a uniform distribution
	double uniformRandom = zeroone();

#ifdef Debug_TRS
	cout << "fBetaPossible = " << fBetaPossible << endl;
	cout << "fBetaCurrent = " << storedFBetaCurrent << endl;
	cout << "pBetaPossible = " << pBetaPossible << endl;
	cout << "pBetaCurrent = " << storedPBetaCurrent << endl;
	cout << "ratio = " << ratio << " and uniformRandom = " << uniformRandom << endl;
#endif


// This is the Metropolis step

	if (alpha > uniformRandom) {

#ifdef Debug_TRS
		cout << "\n \n \t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t  @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  Change Beta @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n \n" << endl;
#endif

		Beta->setChangeStatus(true);
		ccd.resetBeta();
		ccd.setBeta(*betaPossible);
		storedFBetaCurrent = ccd.getLogLikelihood();
		storedPBetaCurrent = ccd.getLogPrior();



	} else{

#ifdef Debug_TRS
		cout << "##############  Reject Beta ##################" << endl;
#endif
		Beta->setChangeStatus(false);

		Beta->restore();
	}

	return alpha;


}


double MHRatio::getHastingsRatio(Parameter * Beta, Parameter * Beta_Hat, Eigen::MatrixXf PrecisionMatrix){

	int betaLength = Beta->getSize();

	Eigen::VectorXf betaProposal(betaLength);

	Eigen::VectorXf betaCurrent(betaLength);

	Eigen::VectorXf beta_hat(betaLength);

	Eigen::VectorXf betaHat_minus_current(betaLength);

	Eigen::VectorXf betaHat_minus_proposal(betaLength);

	Eigen::VectorXf precisionDifferenceProduct_proposal(betaLength);

	Eigen::VectorXf precisionDifferenceProduct_current(betaLength);


	for (int i = 0; i< betaLength; i++){
		betaProposal(i) = Beta->get(i);
		betaCurrent(i) = Beta->getStored(i);
		beta_hat(i) = Beta_Hat->get(i);
	}

	betaHat_minus_current = beta_hat - betaCurrent;
	betaHat_minus_proposal = beta_hat - betaProposal;

	precisionDifferenceProduct_proposal = PrecisionMatrix*betaHat_minus_proposal;
	precisionDifferenceProduct_current = PrecisionMatrix*betaHat_minus_current;

	double numerator = betaHat_minus_current.dot(precisionDifferenceProduct_current);

	double denominator = betaHat_minus_proposal.dot(precisionDifferenceProduct_proposal);






	cout << "Printing Hessian in gethasingsratio" << endl;

	for (int i = 0; i < betaLength; i ++) {
		cout << "[";
			for (int j = 0; j < betaLength; j++) {
				cout << PrecisionMatrix(i,j) << ", ";
			}
		cout << "]" << endl;
		}


	return(0.5*(numerator - denominator)); // log scale
}


double MHRatio::min(double value1, double value2) {
	if (value1 > value2) {
		return value2;
	} else {
		return value1;
	}

}

}
