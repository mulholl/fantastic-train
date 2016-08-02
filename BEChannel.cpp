#include "BEChannel.hpp"

using namespace std;

/* Set the erasure probability */
void BEChannel::channelParam(const float e){
	/* Altering the erasure probability could cause the currently inputs
	 * and outputs currently stored to become invalid, so we check if
	 * we need to update them (i.e., whether the new erasure probability 
	 * is different to the current one).
	 */
	 if (e == epsilon){
	 	/* If the new erasure probability is the same as the old one,
	 	 * we don't need to do anything */
	 	return;
	 }
	 else {
	 	/* Otherwise, we need to update epsilon and resimulate transmission
	 	 * of TX */
	 	epsilon = e;
	 	useChannel(TX);
	 }
}

/* Return the erasure probability */
float BEChannel::channelParam(){
	return epsilon;
}

/* Return the most recently transmitted vector */
BPSKTXVector BEChannel::TXVector(){
	return TX;
}

/* Return the most recently received vector */
BECRXVector BEChannel::RXVector(){
	return RX;
}

/* Return the number of erasures in the most recently received vector */
unsigned int BEChannel::numErrors(){
	return RXErrors;
}

/* Return the indices of the erasures in the most recently received vector */
ErrorIndVector BEChannel::ErrorInds(){
	return ErrInds;
}

/* Simulate erasures over the BEC */
BECRXVector BEChannel::useChannel(const BPSKTXVector &toTransmit){

	/* The BEChannel class stores the most RXently transmitted and RXeived
	 * vectors and some info. about them, so we need to clear/modify
	 * some variables before simulating the next transmission 
	 */

	/* Set the transmitted vector TX to be toTrasmit */
	// TX = toTransmit;

	/* Set the received vector RX to be the same as the new transmitted
	 * vector TX - this will be modified later by randomly erasing some
	 * of the bit values
	 */
	// RX = TX;
	 RX.reserve(toTransmit.size());

	/* Set the number of errors in the RXeived vector to 0 */
	RXErrors = 0;

	/* Clear the vector storing the indices of erased bits */
	// ErrInds.resize(0);
	ErrInds = ErrorIndVector(toTransmit.size(), 0);

	/* Now we can simulate transmission of the vector TX over the 
	 * channel 
	 */

	unsigned int dist = 0;

	// BPSKTXVector::iterator RX_it_begin = RX.begin();
	BPSKTXVector::const_iterator TX_it_end = toTransmit.end();

	/* Generate erased bits */
	for (BPSKTXVector::const_iterator it = toTransmit.begin(); it < TX_it_end; ++it){
		dist++;
		if (rand() < (RAND_MAX + 1.0) * epsilon){
		// if (uniformRDist(RNG) < epsilon){
			// unsigned int dist = distance(toTransmit.begin(), it);
			// *(RX_it_begin + dist) = 0; // 0 used as the BPSK value for an erased bit
			RX.push_back(0);
			RXErrors++; // Increment the number of erasures
			// ErrInds.push_back(dist); // Store the index of this erasure
			ErrInds[RXErrors] = dist;
		}
		else {
			RX.push_back(toTransmit[dist]);
		}
	}

	ErrInds.resize(RXErrors);

	// cout << "Error rate: " << (float) RXErrors / (float) RX.size() << endl;

	return RX;
}

// /* Constructor */
// BEChannel::BEChannel(const float e, mt19937 &RNGIn) : uniformRDist(uniform_real_distribution<float>(0.0, 1.0)) {
// 	RNG = RNGIn;
// 	epsilon = e;
// 	TX.resize(0);
// 	RX.resize(0);
// }

/* Constructor */
BEChannel::BEChannel(const float e) {
	// RNG = RNGIn;
	epsilon = e;
	TX.resize(0);
	RX.resize(0);
}