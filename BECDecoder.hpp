/* BECDecoder class implements a decoder for LDPC codes over the BEC */

#ifndef BEC_DECODER_HPP
#define BEC_DECODER_HPP

#include <Eigen/SparseCore>
#include <vector>
#include <iostream>
#include <cmath>
#include <list>
#include <ctime>
#include <set>
#include <utility>

#include <algorithm>
#include <functional>

#include "BEChannel.hpp"
#include "Modulators.hpp"
#include "VectorTypes.hpp"
#include "VectorUtilities.hpp"

class BECDecoder{
	public:
		BPSKRXVector decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs, bool dummy); // Decode the vector RX_in - (N.B. TX is not currently used as the BEC decoder should never make the wrong decision when correcting an erasure)
		// BPSKRXVector decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs); // Decode the vector RX_in - (N.B. TX is not currently used as the BEC decoder should never make the wrong decision when correcting an erasure)
		// std::vector<unsigned int> decode(const BECRXVector &RX_in, const BPSKTXVector &TX, BPSKRXVector &RX_out);
		// BECRXVector singleIteration(const BECRXVector &in);
		void reset(); // Reset the decoder (i.e., discard all output information such as SERs etc)
		float SER(std::vector<float> &SERVector); // Return a vector of SERs after each iteration
		float SER(); // return the SER after maxIts iterations
		float SER(const unsigned int a); //Return the SER after a iterations
		float FER(std::vector<float> &SERVector); // Return a vector of FERs after each iteration
		float FER(); // Return the FER after maxIts iterations
		float FER(const unsigned int a); // Return the FER after a iterations
		unsigned int numFailures(); // Return the number of incorrectly decoded frames so far
		void codewordsDecodedEachIt(std::vector<unsigned int> &numDecodedEachIt, std::vector<float> &percDecodedEachIt); // Find number and percentage of codewords that have been decoded each iteration
		BECDecoder(Eigen::SparseMatrix<bool,Eigen::ColMajor> edgeList_in, const std::vector<unsigned int> &VNDegrees_in, const std::vector<unsigned int> &CNDegrees_in, const unsigned int i); // Constructor
		// BECDecoder(std::vector< std::vector<unsigned int> > edgeListRowMajor_in, std::vector< std::vector<unsigned int> > edgeListColMajor_in, const std::vector<unsigned int> &VNDegrees_in, const std::vector<unsigned int> &CNDegrees_in, const unsigned int i); // Constructor
		~BECDecoder();
	private:
		/* Sparse matrices containing the edge list for the parity-check matrix */
		Eigen::SparseMatrix<bool,Eigen::ColMajor> edgeListColMajorSp;
		Eigen::SparseMatrix<bool,Eigen::RowMajor> edgeListRowMajorSp;
		std::vector< std::vector<unsigned int> > edgeListColMajor;
		std::vector< std::vector<unsigned int> > edgeListRowMajor;
		unsigned int maxIts; // Maximum number of decoder iterations
		unsigned int numSymbolsDecoded; // The number of symbols that have so far been decoded (whether correctly or incorrectly)
		unsigned int numFramesDecoded; // The number of frames that have so far been decoded (whether correctly or incorrectly)
		unsigned int numFrameFailures; // The number of frames that have so far been incorrectly decoded
		unsigned int numCNs;
		unsigned int numVNs;
		unsigned int maxVNDegree;
		unsigned int minVNDegree;
		unsigned int maxCNDegree;
		unsigned int minCNDegree;
		unsigned int *VNDegrees;
		unsigned int *CNDegrees;
		bool *Connectivity;
		unsigned int *RowConnectivity, *ColConnectivity;
		unsigned int *B;
		std::vector<unsigned int> decodedEachIt; // Vector containing the number of frames correctly decoded for each iteration
		std::vector<unsigned int> symbolErrorsRemaining; // Vector containing the number of symbol errors remaining after each iteration
		std::vector<unsigned int> frameErrorsRemaining; // Vector containing the number of frame errors remaining after each iteration
};

#endif