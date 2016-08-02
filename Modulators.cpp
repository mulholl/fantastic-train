#include "Modulators.hpp"

using namespace std;

BPSKTXVector BPSKMod(const BinTXVector &vecIn){
	BPSKTXVector vecOut;

	BinTXVector::const_iterator endIt = vecIn.end();

	for (BinTXVector::const_iterator it = vecIn.begin(); it < endIt; ++it){
		vecOut.push_back(-(2 * (int)(*it) - 1));
	}

	return vecOut;
}

BECRXVector BPSKDemod(const BECBPSKRXVector &vecIn){
	BECRXVector vecOut;

	BECBPSKRXVector::const_iterator endIt = vecIn.end();

	for (BECBPSKRXVector::const_iterator it = vecIn.begin(); it < endIt; ++it){
		vecOut.push_back(- (*it - 1) / 2);
	}

	return vecOut;
}

// vector<bool> BPSKMod