#include "BECDecoder.hpp"

using namespace std;
using namespace Eigen;

double elapsed_secs_decode = 0;
double elapsed_secs_1 = 0.0;
double elapsed_secs_2 = 0.0;
double elapsed_secs_3 = 0.0;
double elapsed_secs_4 = 0.0;
double elapsed_secs_5 = 0.0;
double elapsed_secs_6 = 0.0;

BPSKRXVector BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs){
	clock_t decode_begin = clock();
	clock_t 
	elapsed_secs_decode = 0;
	BPSKRXVector RX;

	unsigned int break_count = 0;

	unsigned int erasuresRemaining = 0; // The number of remaining uncorrected erasures in the current codeword
	unsigned int erasuresCorrected = 0;

	vector<unsigned int> CurrentCWsymbolErrorsRemaining(maxIts + 1, 0); // Stores the number of bits in the current codeword which are uncorrected after each iteration (starting with iteration 0)
	vector<unsigned int> CurrentCWframeErrorsRemaining(maxIts + 1, 0); // Stores the number of frames in the current codeword which are uncorrected after each iteration (starting with iteration 0) - the codeword is the frame so each value here is either one or zero, the type of the vector is unsigned int so it can easily be added to another vector later

	vector<unsigned int>::iterator symbolErrorsRemainingIt = CurrentCWsymbolErrorsRemaining.begin();
	vector<unsigned int>::iterator frameErrorsRemainingIt = CurrentCWframeErrorsRemaining.begin();

	list<unsigned int> errorIndices; // Indices of the bits which are currently uncorrected

	bool frameError = false; // At each iteration this indicates whether the frame is correctd or uncorrected

	numSymbolsDecoded += RX_in.size();	

 	/* Calculate and record the initial number of symbol and frame errors */
	for (BECRXVector::const_iterator it = RX_in.begin(); it < RX_in.end(); ++it){
		if (*it == 0.5){
			erasuresRemaining++;
			(*symbolErrorsRemainingIt)++;
			frameError = true;

			errorIndices.push_back(distance(RX_in.begin(), it));
		}
	}

	// cout << "Initial erasures: " << erasuresRemaining << endl;

		if (frameError){
			(*frameErrorsRemainingIt)++;
			decodedEachIt[0]++;
		}
		else {
		}	

	symbolErrorsRemainingIt++;
	frameErrorsRemainingIt++;

	unsigned int m = edgeListColMajor.rows(); // The number of CNs
	unsigned int N = edgeListColMajor.cols(); // The number of VNs

	unsigned int i;
	float edgeVal, tmpVal;

	bool allKnown;

	// cout << "erasuresRemaining = " << erasuresRemaining << endl;

	// Initialization - Johnson, Algorithm 2.1 lines 3-5
	BECRXVector M = RX_in;

	clock_t begin_CN, begin_CN_2, begin_CN_3, begin_CN_4, begin_CN_5;
	clock_t end_CN, end_CN_2, end_CN_3, end_CN_4, end_CN_5;

	unsigned int l; // Iteration counter

	bool correctionsThisIt;

	SparseMatrix<float> EdgeMessages;
	EdgeMessages.resize(0, 0);
	EdgeMessages.resize(edgeListColMajor.rows(), edgeListColMajor.cols());	

	// unsigned int pointfives = 0;
	// unsigned int notpointfives = 0;

	for (l = 0; l < maxIts; ++l){

	vector< Triplet<float> > vec;

		correctionsThisIt = false;

		begin_CN = clock();

		unsigned int count_ji = 0;

		/* Check node messages - Johnson, Algorithm 2.1 lines 9-17 */
		for (unsigned int j = 0; j < m; ++j){
			/* Get Bj, the list of VNs connected to CN j */
			vector<unsigned int> Bj;

			unsigned int numUnknown = 0;

			begin_CN_4 = clock();

			for (SparseMatrix<bool, RowMajor>::InnerIterator it(edgeListRowMajor, j); it; ++it){
				Bj.push_back(it.col());
			}

			for (vector<unsigned int>::iterator i = Bj.begin(); i < Bj.end(); ++i){
				/* Check whether all messages into CN j other than the one coming from
				 * VN i are known
				 */

				bool allKnown = true;
				unsigned int idash;
				float sum = 0;

				bool do_exit = false;

				unsigned int counter = 0;
				unsigned int falseCount = 0;

				for (vector<unsigned int>::iterator it = Bj.begin(); it < Bj.end(); ++it){
					idash = *it;
					vector<float>::iterator M_it = M.begin() + idash;
					if (idash == *i){
						continue;
					}
					else if (*M_it == 0.5){
						allKnown = false;
						falseCount++;
						break;
					}
					else {
						sum = fmod(sum + *M_it, 2);
						counter++;
					}
				}

				if (allKnown) {
					vec.push_back(Triplet<float>(j, *i, 100.00)); // Currently the value 100 is used here rather than the actual value because the default value of elements of Eigen's Sparse Matrices is 0
				}
				else {
					vec.push_back(Triplet<float>(j, *i, 0.5));
				}
			}
		}

		EdgeMessages.setFromTriplets(vec.begin(), vec.end());

		/* Variable node messages - Johnson, Algorithm 2.1 lines 19-25 */
		list<unsigned int>::iterator i = errorIndices.begin();

		bool madeCorrection;

		unsigned int tmp = errorIndices.size();
		unsigned int counter = 0;

		frameError = false;

		while (i != errorIndices.end()){
			madeCorrection = false;

			/* Go through each of the CNs connected to the current VN */
			for (SparseMatrix<bool, ColMajor>::InnerIterator it(edgeListColMajor, *i); it; ++it){
				unsigned int j = it.row();
				float Eji = EdgeMessages.coeffRef(j, *i); // The message on the edge connected to CN j

				/* If the message is not an erasure, we can correct the VN bit */
				if (Eji == 100){
					M[*i] = Eji;
					i = errorIndices.erase(i);
					erasuresRemaining--;
					erasuresCorrected++;
					madeCorrection = true;
					correctionsThisIt = true;
					break;
				}
			}

			if (!madeCorrection){
				i++;
					(*symbolErrorsRemainingIt)++;
					frameError = true;					
			}
		}

		/* Update the vector tracking frame errors and increment the symbol/frame error vector iterators */
		if (frameError){
			(*frameErrorsRemainingIt)++;
		}
		symbolErrorsRemainingIt++;		
		frameErrorsRemainingIt++;

		/* Stopping criterion - Johnson, Algorithm 2.1 lines 27-28 
		 * (erasuresRemaining == 0 is the same as "all M_i known")
		 */
		if (erasuresRemaining == 0){
			decodedEachIt[l+1]++;
			unsigned int tmpSymbErrs = *symbolErrorsRemainingIt;
			unsigned int tmpFrameErrs = *frameErrorsRemainingIt;
			while (symbolErrorsRemainingIt < CurrentCWsymbolErrorsRemaining.end()){
				*symbolErrorsRemainingIt = tmpSymbErrs;
				*frameErrorsRemainingIt = tmpFrameErrs;
				++symbolErrorsRemainingIt;
				++frameErrorsRemainingIt;
			}
			break;
		}
		else if (!correctionsThisIt){
			// break_count++;
			// if (break_count == 5){
				cout << "Breaking due to no corrections made" << endl;
				// cout << "DONE! l = " << l << endl;
				// decodedEachIt[maxIts]++;
				// break;
			// }
			// decodedEachIt[l]++;
			unsigned int tmpSymbErrs = *symbolErrorsRemainingIt;
			unsigned int tmpFrameErrs = *frameErrorsRemainingIt;
			while (symbolErrorsRemainingIt < CurrentCWsymbolErrorsRemaining.end()){
				*symbolErrorsRemainingIt = tmpSymbErrs;
				*frameErrorsRemainingIt = tmpFrameErrs;
				++symbolErrorsRemainingIt;
				++frameErrorsRemainingIt;
			}
			break;
		}
		// else {
			// cout << "\t\t\tl = " << l << endl;
		// }
	}

	numErrs = erasuresRemaining;

	if (erasuresRemaining > 0){
		decodedEachIt[maxIts+1]++;
	}
	
	// cout << "Erasures remaining after " << l+1 << " iterations: " << erasuresRemaining << endl;
	// cout << "elapsed_secs_CN = " << elapsed_secs_CN << endl;
	// cout << "elapsed_secs_CN_2 = " << elapsed_secs_CN_2 << endl;
	// cout << "elapsed_secs_CN_3 = " << elapsed_secs_CN_3 << endl;
	// cout << "elapsed_secs_CN_4 = " << elapsed_secs_CN_4 << endl;
	// cout << "elapsed_secs_CN_5 = " << elapsed_secs_CN_5 << endl;
	// cout << "elapsed_secs_VN = " << elapsed_secs_VN << endl;


	clock_t decode_end = clock();


	elapsed_secs_decode += double(decode_end - decode_begin) / CLOCKS_PER_SEC;
	numFramesDecoded++;	
	// if (l == maxIts){
		// decodedEachIt++;
	// }
	// decodedEachIt[l]++;
	cout << "elapsed_secs_decode = " << elapsed_secs_decode << endl;
	cout << "average decode time (" << numFramesDecoded << " decodes so far) = " << elapsed_secs_decode / (double)numFramesDecoded << endl;
	cout << "Its used: " << endl;
	for (int it = 0; it <= maxIts; ++it){
		cout << "\t" << it;
	}
	cout << endl;
	for (vector<unsigned int>::iterator it = decodedEachIt.begin(); it < decodedEachIt.end(); ++it){
		cout << "\t" << *it;
	}
	cout << endl;
	for (vector<unsigned int>::iterator it = decodedEachIt.begin(); it < decodedEachIt.end(); ++it){
		cout << "\t" << 100 * (float)(*it) / (float)(numFramesDecoded);
	}
	cout << endl;

	symbolErrorsRemaining = symbolErrorsRemaining + CurrentCWsymbolErrorsRemaining;
	frameErrorsRemaining = frameErrorsRemaining + CurrentCWframeErrorsRemaining;

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

	decodedEachIt.resize(maxIts + 2, 0);
	symbolErrorsRemaining.resize(maxIts + 1, 0);
	frameErrorsRemaining.resize(maxIts + 1, 0);
}

float BECDecoder::SER(vector<float> &SERVector){
	vector<float> tmp(symbolErrorsRemaining.begin(), symbolErrorsRemaining.end());
	SERVector = tmp / ((float) numSymbolsDecoded);

	return *(SERVector.end());
}

float BECDecoder::SER(){
	return (float)(*(symbolErrorsRemaining.end())) / (float) (numSymbolsDecoded);
}

float BECDecoder::SER(const unsigned int a){
	return (float)(*(symbolErrorsRemaining.begin() + a)) / (float) (numSymbolsDecoded);
}

float BECDecoder::FER(vector<float> &FERVector){
	vector<float> tmp(frameErrorsRemaining.begin(), frameErrorsRemaining.end());
	FERVector = tmp / (float) numFramesDecoded;

	return *(FERVector.end());
}

float BECDecoder::FER(){
	return (float)(*(frameErrorsRemaining.end())) / (float) (numFramesDecoded);
}

float BECDecoder::FER(const unsigned int a){
	return (float)(*(frameErrorsRemaining.begin() + a)) / (float) (numFramesDecoded);
}

/* Finds the number and percentage of all codewords that have been decoded after each iteration,
 * from 0 to maxIts iterations, the final element of each of the vectors represents the
 * number/percentage of codewords that were not successfully decoded after maxIts iterations
 */
void BECDecoder::codewordsDecodedEachIt(vector<unsigned int> &numDecodedEachIt, vector<float> &percDecodedEachIt){
	numDecodedEachIt = decodedEachIt;

	vector<float> tmp(decodedEachIt.begin(), decodedEachIt.end());

	percDecodedEachIt = (100 * tmp) / (float) numFramesDecoded;
}

BECDecoder::BECDecoder(Eigen::SparseMatrix<bool,ColMajor> edgeList_in, const unsigned int i){
	edgeListColMajor = edgeList_in;
	edgeListRowMajor = SparseMatrix<bool,RowMajor>(edgeListColMajor);
	maxIts = i;

	// numSymbolsDecoded = 0;
	// numFramesDecoded = 0;

	// symbolErrorsRemaining = vector<unsigned int>(maxIts + 1, 0);
	// frameErrorsRemaining = vector<unsigned int>(maxIts + 1, 0);

	reset();
}

// unsigned int BECDecoder::countErrors(const BPSKTXVector &CorrectCW){
// 	BPSKRXVector::iterator it2;

// 	unsigned int symbolErrs = 0;
// 	BPSKTXVector tmp = CorrectCW;

// 	for (BPSKTXVector::const_iterator it1 = CorrectCW.begin(); it1 < CorrectCW.end(); ++it1){
// 		it2 = DecoderOutput.begin() + distance(CorrectCW.begin(), it1);

// 		if ((*it1) != (*it2)){
// 			symbolErrs++;
// 		}
// 	}

// 	return symbolErrs;
// }