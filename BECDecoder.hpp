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
		BPSKRXVector decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs);
		std::vector<unsigned int> decode(const BECRXVector &RX_in, const BPSKTXVector &TX, BPSKRXVector &RX_out);
		BECRXVector singleIteration(const BECRXVector &in);
		void reset();
		float SER(std::vector<float> &SERVector);
		float SER();
		float SER(const unsigned int a);
		float FER(std::vector<float> &SERVector);
		float FER();
		float FER(const unsigned int a);
		void codewordsDecodedEachIt(std::vector<unsigned int> &numDecodedEachIt, std::vector<float> &percDecodedEachIt);
		BECDecoder(Eigen::SparseMatrix<bool,Eigen::ColMajor> edgeList_in, const unsigned int i);
	private:
		Eigen::SparseMatrix<bool,Eigen::ColMajor> edgeListColMajor;
		Eigen::SparseMatrix<bool,Eigen::RowMajor> edgeListRowMajor;
		unsigned int maxIts;
		BECRXVector DecoderInput;
		BPSKTXVector CorrectCw;
		BPSKRXVector DecoderOutput;
		unsigned int numSymbolsDecoded;
		unsigned int numFramesDecoded;
		unsigned int cumulativeUnfixedSymbolErrors;
		unsigned int cumulativeUnfixedFrameErrors;
		unsigned int countErrors(const BPSKTXVector &);
		std::vector<unsigned int> decodedEachIt;
		std::vector<unsigned int> symbolErrorsRemaining;
		std::vector<unsigned int> frameErrorsRemaining;
};

#endif