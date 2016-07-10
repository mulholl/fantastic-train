#include "BECDecoder.hpp"

using namespace std;
using namespace Eigen;

BPSKRXVector BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs){
	BPSKRXVector RX;

	SparseMatrix<float> EdgeMessages;
	EdgeMessages.resize(edgeListColMajor.rows(), edgeListColMajor.cols());

	/* First count the number of erasures so that later we can figure out when
	 * to stop iterating 
	 */
	unsigned int erasuresRemaining = 0;
	unsigned int erasuresCorrected = 0;

	list<unsigned int> errorIndices;

	for (BECRXVector::const_iterator it = RX_in.begin(); it < RX_in.end(); ++it){
		if (*it == 0.5){
			erasuresRemaining++;
			errorIndices.push_back(distance(RX_in.begin(), it));
		}
	}

	unsigned int m = edgeListColMajor.rows(); // The number of CNs
	unsigned int N = edgeListColMajor.cols(); // The number of VNs

	unsigned int i;
	float edgeVal, tmpVal;

	bool allKnown;

	// cout << "erasuresRemaining = " << erasuresRemaining << endl;

	// Initialization - Johnson, Algorithm 2.1 lines 3-5
	BECRXVector M = RX_in;

	double elapsed_secs_CN = 0.0;
	double elapsed_secs_CN_2 = 0.0;
	double elapsed_secs_CN_3 = 0.0;
	double elapsed_secs_VN = 0.0;

	unsigned int l;

	for (l = 0; l < maxIts; ++l){

		clock_t begin_CN = clock();

		/* Check node messages - Johnson, Algorithm 2.1 lines 9-17 */
		for (unsigned int j = 0; j < m; ++j){
			/* Get Bj, the list of VNs connected to CN j */
			vector<unsigned int> Bj;

			unsigned int numUnknown = 0;


			clock_t begin_CN_2 = clock();			

			for (SparseMatrix<bool, RowMajor>::InnerIterator it(edgeListRowMajor, j); it; ++it){
				Bj.push_back(it.col());
			}

			clock_t end_CN_2 = clock();

			elapsed_secs_CN_2 += double(end_CN_2 - begin_CN_2) / CLOCKS_PER_SEC;	

			// unsigned int Bj_ind;
			// unsigned int i, idash;

			clock_t begin_CN_3 = clock();

			for (vector<unsigned int>::iterator i = Bj.begin(); i < Bj.end(); ++i){
			// for (Bj_ind = 0; Bj_ind < Bj.size(); ++Bj_ind){
				// i = Bj[Bj_ind];

				/* Check whether all messages into CN j other than the one coming from
				 * VN i are known
				 */

				bool allKnown = true;
				unsigned int idash;
				float sum = 0;

				unsigned int counter = 0;
				// cout << "Bj.size() = " << Bj.size() << endl;

				for (vector<unsigned int>::iterator it = Bj.begin(); it < Bj.end(); ++it){
					idash = *it;
					vector<float>::iterator M_it = M.begin() + idash;
					if (idash == *i){
						continue;
					}
					else if (*M_it == 0.5){
						allKnown = false;
						break;
					}
					else {
						sum = fmod(sum + *M_it, 2);
						counter++;
					}
				}

				// cout << "counter = " << counter << endl;
				// exit(0);

				if (allKnown) {
					EdgeMessages.coeffRef(j, *i) = sum;
					// cout << "sum = " << sum << endl;
				}
				else {
					EdgeMessages.coeffRef(j, *i) = 0.5;
				}
			}

			clock_t end_CN_3 = clock();

			elapsed_secs_CN_3 += double(end_CN_3 - begin_CN_3) / CLOCKS_PER_SEC;			
		}

		clock_t end_CN = clock();

		elapsed_secs_CN += double(end_CN - begin_CN) / CLOCKS_PER_SEC;		

		clock_t begin_VN = clock();		

		/* Variable node messages - Johnson, Algorithm 2.1 lines 19-25 */
		list<unsigned int>::iterator i = errorIndices.begin();

		bool madeCorrection;

		unsigned int tmp = errorIndices.size();
		unsigned int counter = 0;

		while (i != errorIndices.end()){
			madeCorrection = false;

			/* Go through each of the CNs connected to the current VN */
			for (SparseMatrix<bool, ColMajor>::InnerIterator it(edgeListColMajor, *i); it; ++it){
				unsigned int j = it.row();
				float Eji = EdgeMessages.coeffRef(j, *i); // The message on the edge connected to CN j

				/* If the message is not an erasure, we can correct the VN bit */
				if (Eji != 0.5){
					M[*i] = Eji;
					i = errorIndices.erase(i);
					erasuresRemaining--;
					madeCorrection = true;
					break;
				}
			}

			counter++;

			if (!madeCorrection){
				i++;
			}
		}

		cout << "went through " << counter << " of " << tmp << " erasure." << endl;

		clock_t end_VN = clock();	

		elapsed_secs_VN += double(end_VN - begin_VN) / CLOCKS_PER_SEC;				


		// cout << "count = " << count << endl;
		// exit(0);

		// cout << "After iteration #" << l+1 << ", " << erasuresRemaining << " errors remain." << endl;

		/* Stopping criterion - Johnson, Algorithm 2.1 lines 27-28 
		 * (erasuresRemaining == 0 is the same as "all M_i known")
		 */
		if (erasuresRemaining == 0){
			break;
		}
	}

	numErrs = erasuresRemaining;
	
	cout << "Erasures remaining after " << l << " iterations: " << erasuresRemaining << endl;
	cout << "elapsed_secs_CN = " << elapsed_secs_CN << endl;
	cout << "elapsed_secs_CN_2 = " << elapsed_secs_CN_2 << endl;
	cout << "elapsed_secs_CN_3 = " << elapsed_secs_CN_3 << endl;
	cout << "elapsed_secs_VN = " << elapsed_secs_VN << endl;

	// exit(0);

	return RX;	
}

/* This alternate form of decode() returns a vector which contains as its zeroth
 * element, the number of errors in the received vector, followed by the number of
 * errors remaining after each iteration (i.e., symbolErrs[i] contains the number of
 * errors remaining after i decoder iterations)
 */
// vector<unsigned int> BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, BPSKRXVector &RX_out){
// 	vector<unsigned int> symbolErrs;
// 	symbolErrs.reserve(maxIts);

// 	for (unsigned int it = 0; it < maxIts; ++it){
// 		symbolErrs.push_back(countErrors());
// 	}

// 	return symbolErrs;
// }

// BECDecoder::Initialize();

// BECRXVector BECDecoder::singleIteration(const BECRXVector &in){

// }

void BECDecoder::reset(){
	numSymbolsDecoded = 0;
	numFramesDecoded = 0;
	cumulativeUnfixedSymbolErrors = 0;
	cumulativeUnfixedFrameErrors = 0;
}

float BECDecoder::SER(){
	float symbolErrorRate = (float) cumulativeUnfixedSymbolErrors / (float) numSymbolsDecoded;
	return symbolErrorRate;
}

float BECDecoder::FER(){
	float frameErrorRate = (float) cumulativeUnfixedFrameErrors / (float) numFramesDecoded;
	return frameErrorRate;
}

BECDecoder::BECDecoder(Eigen::SparseMatrix<bool,ColMajor> edgeList_in, const unsigned int i){
	edgeListColMajor = edgeList_in;
	edgeListRowMajor = SparseMatrix<bool,RowMajor>(edgeListColMajor);
	maxIts = i;

	reset();
}

unsigned int BECDecoder::countErrors(const BPSKTXVector &CorrectCW){
	BPSKRXVector::iterator it2;

	unsigned int symbolErrs = 0;
	BPSKTXVector tmp = CorrectCW;

	for (BPSKTXVector::const_iterator it1 = CorrectCW.begin(); it1 < CorrectCW.end(); ++it1){
		it2 = DecoderOutput.begin() + distance(CorrectCW.begin(), it1);

		if ((*it1) != (*it2)){
			symbolErrs++;
		}
	}

	return symbolErrs;
}