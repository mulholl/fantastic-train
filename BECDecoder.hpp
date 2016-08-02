/* BECDecoder class implements a decoder for LDPC codes over the BEC */

#ifndef BEC_DECODER_HPP
#define BEC_DECODER_HPP

#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <set>
#include <utility>

#include <algorithm>
#include <functional>

#include "BEChannel.hpp"
#include "Modulators.hpp"
#include "VectorTypes.hpp"
#include "VectorUtilities.hpp"
#include "EdgeList.hpp"

class BECDecoder{
	public:
		BPSKRXVector decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs); // Decode the vector RX_in - (N.B. TX is not currently used as the BEC decoder should never make the wrong decision when correcting an erasure)
		void reset(); // Reset the decoder (i.e., discard all output information such as SERs etc)
		float SER(std::vector<float> &SERVector); // Return a vector of SERs after each iteration
		float SER(); // return the SER after maxIts iterations
		float SER(const unsigned int a); //Return the SER after a iterations
		float FER(std::vector<float> &SERVector); // Return a vector of FERs after each iteration
		float FER(); // Return the FER after maxIts iterations
		float FER(const unsigned int a); // Return the FER after a iterations
		unsigned int numFailures(); // Return the number of incorrectly decoded frames so far
		void codewordsDecodedEachIt(std::vector<unsigned int> &numDecodedEachIt, std::vector<float> &percDecodedEachIt); // Find number and percentage of codewords that have been decoded each iteration
		BECDecoder(const EdgeList * EL_in, const unsigned int &maxIts_in);
		~BECDecoder();
	private:
		const EdgeList *EL;
		unsigned int maxIts; // Maximum number of decoder iterations
		unsigned int numSymbolsDecoded; // The number of symbols that have so far been decoded (whether correctly or incorrectly)
		unsigned int numFramesDecoded; // The number of frames that have so far been decoded (whether correctly or incorrectly)
		unsigned int numFrameFailures; // The number of frames that have so far been incorrectly decoded
		int numCNs;
		int numVNs;
		int maxVNDegree;
		int minVNDegree;
		int maxCNDegree;
		int minCNDegree;
		int *VNDegrees;
		int *CNDegrees;
		std::vector<unsigned int> decodedEachIt; // Vector containing the number of frames correctly decoded for each iteration
		std::vector<unsigned int> symbolErrorsRemaining; // Vector containing the number of symbol errors remaining after each iteration
		std::vector<unsigned int> frameErrorsRemaining; // Vector containing the number of frame errors remaining after each iteration
};

#endif