#include "Modulators.hpp"

using namespace std;

BPSKTXVector BPSKMod(const BinTXVector &vecIn){
	BPSKTXVector vecOut;

	for (BinTXVector::const_iterator it = vecIn.begin(); it < vecIn.end(); ++it){
		vecOut.push_back(-(2 * (int)(*it) - 1));
	}

	return vecOut;
}

BECRXVector BPSKDemod(const BECBPSKRXVector &vecIn){
	BECRXVector vecOut;

	for (BECBPSKRXVector::const_iterator it = vecIn.begin(); it < vecIn.end(); ++it){
		vecOut.push_back(- (*it - 1) / 2);
	}

	return vecOut;
}

// vector<bool> BPSKMod