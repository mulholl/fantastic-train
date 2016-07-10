#ifndef BINARY_ERASURE_CHANNEL_HPP
#define BINARY_ERASURE_CHANNEL_HPP

#define ERASURE_VALUE 0

#include <vector>
#include <cstdlib>
#include <random>
#include <iostream>

#include "Modulators.hpp"
#include "VectorTypes.hpp"

class BEChannel{
	public:
		void channelParam(const float e); // Set the erasure probability
		float channelParam(); // Return the erasure probability
		BPSKTXVector TXVector(); // Return the most recently transmitted vector
		BECRXVector RXVector(); // Return the most recently received vector
		unsigned int numErrors(); // Return the number of erasures in the most recently received vector
		ErrorIndVector ErrorInds(); // Return the indices of the erasures in the most recently received vector
		BECRXVector useChannel(const BPSKTXVector &toTransmit); // Simulate transmission of toTransmit over the channel
		BEChannel(const float e, std::mt19937 &RNGIn);
	private:
		float epsilon; // Stores the erasure probability for the channel
		BPSKTXVector TX; // Stores the most recently transmitted vecotr
		BECRXVector RX; // Stores the most recently received vector
		unsigned int RXErrors; // Stores the number of erasures in RXVector
		ErrorIndVector ErrInds; // Stores the indices of the erased bits in RXVector
		std::mt19937 RNG; // Mersenne Twister pseudorandum number generator
		std::uniform_real_distribution<float> uniformRDist; // Uniform real distribution
};

#endif