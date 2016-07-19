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

BPSKRXVector BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs, bool dummy){
	unsigned int b = 0;
	float M[numVNs];
	unsigned int numRemainingErrors = 0;
	unsigned int numInitialErrors = 0;


	copy(RX_in.begin(), RX_in.end(), M);
	for (unsigned int i = 0; i < numVNs; ++i){
		if (M[i] == 0.5){
			numInitialErrors++;
		}
	}

	// for (unsigned int i = 0; i < numVNs; ++i){
	// 	M[i] = RX_in[i];
	// 	if (M[i] == 0.5){
	// 		numInitialErrors++;
	// 	}
	// }

	numRemainingErrors = numInitialErrors;
	numSymbolsDecoded+= numVNs;
	numFramesDecoded++;

	unsigned int errorsCorrectedThisIt = 0;

	unsigned int numCorrectedErrs = 0;

	// float EdgeMessages[numCNs * numVNs];
	float *EdgeMessages = new float[numCNs * numVNs];

	// for (unsigned int a = 0; a < numCNs * numVNs; ++a){
	// 	EdgeMessages[a] = -100.00;
	// }

	unsigned int i, i_dash, j, l;
	unsigned int i_it, j_it, i_dash_it;

	symbolErrorsRemaining[0] += numInitialErrors;
	if (numInitialErrors){
		frameErrorsRemaining[0]++;
	}

	/* Johnson Algorithm 2.1 Lines 7-32 */
	for (l = 1; l <= maxIts; ++l){
		errorsCorrectedThisIt = 0;

		/* Johnson Algorithm 2.1 Lines 9-17 */
		for (j = 0; j < numCNs; ++j){

			/* Johnson Algorithm 2.1 Lines 10-16 */
			for (i_it = 0; i_it < CNDegrees[j]; ++i_it){
				i = RowConnectivity[maxCNDegree * j + i_it];

				bool allOtherMessagesIntoCNKnown = true;
				bool mod2_sum = false;

				/* Johnson Algorithm 2.1 lines 11-15 */
				for (i_dash_it = 0; i_dash_it < CNDegrees[j]; ++i_dash_it){
					i_dash = RowConnectivity[maxCNDegree * j + i_dash_it];
					if (i_dash == i){
						continue;
					}
					else if (M[i_dash] == 0.5) {
						allOtherMessagesIntoCNKnown = false;
					}
					else {
						mod2_sum = (M[i_dash] == 0) ? mod2_sum : true;
					}
				}

				if (allOtherMessagesIntoCNKnown){
					EdgeMessages[numVNs * j + i] = 100.0;
				}
				else {
					EdgeMessages[numVNs * j + i] = 0.5;
				}
				/* End Johnson Algorithm 2.1 lines 11-15 */
			}
			/* End Johnson Algorithm 2.1 Lines 10-16 */
		}
		/* End Johnson Algorithm 2.1 Lines 9-17 */

		/* Johnson Algorithm 2.1 Lines 19-25 */
		for (i = 0; i < numVNs; ++i){

			/* Johnson Algorithm 2.1 Lines 20-24 */
			if (M[i] == 0.5){

				/* Johnson Algorithm 2.1 Lines 21-23 */
				for (j_it = 0; j_it < VNDegrees[i]; ++j_it){
					j = ColConnectivity[maxVNDegree * i + j_it];
					if (EdgeMessages[numVNs * j + i] != 0.5){
						M[i] = EdgeMessages[numVNs * j + i];
						numRemainingErrors--;
						numCorrectedErrs++;
						numSymbolsDecoded++;
						errorsCorrectedThisIt++;
						break;
					}
				}
				/* End Johnson Algorithm 2.1 Lines 21-23 */

			}
			/* End Johnson Algorithm 2.1 Lines 20-24 */

		}
		/* Johnson Algorithm 2.1 Lines 20-24 */

		symbolErrorsRemaining[l] += numRemainingErrors;
		frameErrorsRemaining[l]++;

		/* Johnson Algorithm 2.1 Lines 27-28 */
		if (numRemainingErrors == 0){
			decodedEachIt[l]++;
			break;
		}
		/* End Johnson Algorithm 2.1 Lines 27-28 */
		else if (errorsCorrectedThisIt == 0){
			break;
		}
	}
	/* End Johnson Algorithm 2.1 Lines 7-32 */

	if (numRemainingErrors > 0){
		if (l == maxIts){
			frameErrorsRemaining[maxIts+1]++;
			symbolErrorsRemaining[maxIts+1] += numRemainingErrors;
			numFrameFailures++;
			decodedEachIt[maxIts+1]++;
		}
		else {
			for (l = l+1; l <= maxIts; ++l){
				frameErrorsRemaining[l]++;
				symbolErrorsRemaining[l] += numRemainingErrors;
			}
			decodedEachIt[maxIts+1]++;
			numFrameFailures++;
		}
	}

	numErrs = numRemainingErrors;

	delete [] EdgeMessages;

	BPSKRXVector RX;

	RX.assign(M, M+numVNs);
	// RX.resize(numVNs);
	// for (int i = 0; i < numVNs; ++i){
		// RX[i] = M[i];
	// }

	return RX;	
}

// BPSKRXVector BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs){
// 	clock_t decode_begin = clock();
// 	// clock_t ;
// 	elapsed_secs_decode = 0;
// 	BPSKRXVector RX;

// 	unsigned int break_count = 0;

// 	unsigned int erasuresRemaining = 0; // The number of remaining uncorrected erasures in the current codeword
// 	unsigned int erasuresCorrected = 0;

// 	vector<unsigned int> CurrentCWsymbolErrorsRemaining(maxIts + 1, 0); // Stores the number of bits in the current codeword which are uncorrected after each iteration (starting with iteration 0)
// 	vector<unsigned int> CurrentCWframeErrorsRemaining(maxIts + 1, 0); // Stores the number of frames in the current codeword which are uncorrected after each iteration (starting with iteration 0) - the codeword is the frame so each value here is either one or zero, the type of the vector is unsigned int so it can easily be added to another vector later

// 	vector<unsigned int>::iterator symbolErrorsRemainingIt = CurrentCWsymbolErrorsRemaining.begin();
// 	vector<unsigned int>::iterator frameErrorsRemainingIt = CurrentCWframeErrorsRemaining.begin();

// 	list<unsigned int> errorIndices; // Indices of the bits which are currently uncorrected

// 	bool frameError = false; // At each iteration this indicates whether the frame is correctd or uncorrected

// 	numSymbolsDecoded += RX_in.size();	

//  	/* Calculate and record the initial number of symbol and frame errors */
// 	for (BECRXVector::const_iterator it = RX_in.begin(); it < RX_in.end(); ++it){
// 		if (*it == 0.5){
// 			erasuresRemaining++;
// 			(*symbolErrorsRemainingIt)++;
// 			frameError = true;

// 			errorIndices.push_back(distance(RX_in.begin(), it));
// 		}
// 	}

// 	// cout << "Initial erasures: " << erasuresRemaining << endl;

// 		if (frameError){
// 			(*frameErrorsRemainingIt)++;
// 			decodedEachIt[0]++;
// 		}
// 		else {
// 		}	

// 	symbolErrorsRemainingIt++;
// 	frameErrorsRemainingIt++;

// 	unsigned int m = edgeListRowMajor.size(); // The number of CNs
// 	unsigned int N = edgeListColMajor.size(); // The number of VNs

// 	unsigned int i;
// 	float edgeVal, tmpVal;

// 	bool allKnown;

// 	// cout << "erasuresRemaining = " << erasuresRemaining << endl;

// 	// Initialization - Johnson, Algorithm 2.1 lines 3-5
// 	BECRXVector M = RX_in;

// 	clock_t begin_CN, begin_CN_2, begin_CN_3, begin_CN_4, begin_CN_5;
// 	clock_t end_CN, end_CN_2, end_CN_3, end_CN_4, end_CN_5;

// 	unsigned int l; // Iteration counter

// 	bool correctionsThisIt;

// 	std::vector< std::vector<unsigned int> >::iterator colMajorIt;
// 	std::vector< std::vector<unsigned int> >::iterator rowMajorIt;

// 	std::vector< std::vector<float> > EdgeMessages(m, std::vector<float>(N, 0));

// 	// for (std::vector<float>::iterator it = M.begin(); it < M.end(); ++it){
// 	// 	cout << "*it = " << *it <<endl;
// 	// }
// 	// exit(0);

// 	for (l = 0; l < maxIts; ++l){

// 		vector< Triplet<float> > vec;

// 		correctionsThisIt = false;

// 		begin_CN = clock();

// 		unsigned int count_ji = 0;

// 		/* Check node messages - Johnson, Algorithm 2.1 lines 9-17 */
// 		for (unsigned int j = 0; j < m; ++j){
// 			/* Get Bj, the list of VNs connected to CN j */
// 			vector<unsigned int> Bj;

// 			unsigned int numUnknown = 0;

// 			begin_CN_4 = clock();

// 			// for (SparseMatrix<bool, RowMajor>::InnerIterator it(edgeListRowMajor, j); it; ++it){
// 			// 	Bj.push_back(it.col());
// 			// }
// 			Bj = edgeListRowMajor[j];
// 			// cout << "j = " << j << endl;
// 			// for (vector<unsigned int>::iterator it = Bj.begin(); it < Bj.end(); ++it){
// 				// cout << "\t" << *it;
// 			// }
// 			// cout << endl;
// 			// exit(0);

// 			for (vector<unsigned int>::iterator i = Bj.begin(); i < Bj.end(); ++i){
// 				/* Check whether all messages into CN j other than the one coming from
// 				 * VN i are known
// 				 */

// 				 // cout << "*i = " << *i << endl;
// 				 // cout << "M[*i] = " << M[*i] << endl;

// 				bool allKnown = true;
// 				unsigned int idash;
// 				float sum = 0;

// 				bool do_exit = false;

// 				unsigned int counter = 0;
// 				unsigned int falseCount = 0;

// 				for (vector<unsigned int>::iterator it = Bj.begin(); it < Bj.end(); ++it){
// 					idash = *it;
// 					vector<float>::iterator M_it = M.begin() + idash;
// 					// cout << "idash = " << idash << " | *M_it = " << *M_it << "M.size() = " << M.size() << endl;
// 					if (idash == *i){
// 						continue;
// 					}
// 					else if (*M_it == 0.5){
// 						// cout << "setting allKnown = false as *M_it = " << *M_it << endl;
// 						allKnown = false;
// 						falseCount++;
// 						break;
// 					}
// 					else {
// 						// cout << "No change as *M_it = " << *M_it << endl;
// 						sum = fmod(sum + *M_it, 2);
// 						counter++;
// 					}
// 				}


// 				if (allKnown) {
// 					EdgeMessages[j][*i] = 100;
// 					// cout << "set EdgeMessages[" << j << "][" << *i << "] = " << EdgeMessages[j][*i] << " (set to 100) " << endl;
// 					// vec.push_back(Triplet<float>(j, *i, 100.00)); // Currently the value 100 is used here rather than the actual value because the default value of elements of Eigen's Sparse Matrices is 0
// 				}
// 				else {
// 					EdgeMessages[j][*i] = 0.5;
// 					// cout << "set EdgeMessages[" << j << "][" << *i << "] = " << EdgeMessages[j][*i] <<endl;
// 					// vec.push_back(Triplet<float>(j, *i, 0.5));
// 				}
// 			}
// 			// exit(0);
// 		}

// 		/* Variable node messages - Johnson, Algorithm 2.1 lines 19-25 */
// 		list<unsigned int>::iterator errorIndIt = errorIndices.begin();

// 		bool madeCorrection;

// 		unsigned int tmp = errorIndices.size();
// 		unsigned int counter = 0;

// 		frameError = false;

// 		while (errorIndIt != errorIndices.end()){
// 			// unsigned int i = std::distance(errorIndices.begin(), errorIndIt);
// 			madeCorrection = false;

// 			/* Go through each of the CNs connected to the current VN */
// 			// colMajorIt = edgeListColMajor.boolegin() + std::distance(ErrorIndices.begin(), errorIndIt);
// 			std::vector<unsigned int> connectedToCurrentVN = edgeListColMajor[*errorIndIt];

// 			// cout << "VN number " << *errorIndIt << endl;
// 			// for (std::vector<unsigned int>::iterator it = connectedToCurrentVN.begin(); it < connectedToCurrentVN.end(); ++it){
// 			// 	cout << "\t" << *it;
// 			// }
// 			// cout << endl;
// 			// exit(0);

// 			for (std::vector<unsigned int>::iterator j = connectedToCurrentVN.begin(); j < connectedToCurrentVN.end(); ++j){
// 			// for (SparseMatrix<bool, ColMajor>::InnerIterator it(edgeListColMajor, *errorIndIt); it; ++it){
// 				// unsigned int j = it.row();
// 				// float Eji = EdgeMessages.coeffRef(j, *errorIndIt); // The message on the edge connected to CN j
// 				float Eji = EdgeMessages[*j][*errorIndIt];

// 				/* If the message is not an erasure, we can correct the VN bit */
// 				if (Eji == 100){
// 					M[*errorIndIt] = Eji;
// 					errorIndIt = errorIndices.erase(errorIndIt);
// 					erasuresRemaining--;
// 					erasuresCorrected++;
// 					madeCorrection = true;
// 					correctionsThisIt = true;
// 					break;
// 				}
// 			}

// 			if (!madeCorrection){
// 				errorIndIt++;
// 					(*symbolErrorsRemainingIt)++;
// 					frameError = true;					
// 			}
// 		}

// 		/* Update the vector tracking frame errors and increment the symbol/frame error vector iterators */
// 		if (frameError){
// 			(*frameErrorsRemainingIt)++;
// 		}
// 		symbolErrorsRemainingIt++;		
// 		frameErrorsRemainingIt++;

// 		/* Stopping criterion - Johnson, Algorithm 2.1 lines 27-28 
// 		 * (erasuresRemaining == 0 is the same as "all M_i known")
// 		 */
// 		if (erasuresRemaining == 0){
// 			decodedEachIt[l+1]++;
// 			unsigned int tmpSymbErrs = *symbolErrorsRemainingIt;
// 			unsigned int tmpFrameErrs = *frameErrorsRemainingIt;
// 			while (symbolErrorsRemainingIt < CurrentCWsymbolErrorsRemaining.end()){
// 				*symbolErrorsRemainingIt = tmpSymbErrs;
// 				*frameErrorsRemainingIt = tmpFrameErrs;
// 				++symbolErrorsRemainingIt;
// 				++frameErrorsRemainingIt;
// 			}
// 			break;
// 		}
// 		else if (!correctionsThisIt){
// 			// break_count++;
// 			// if (break_count == 5){
// 				cout << "Breaking due to no corrections made" << endl;
// 				// cout << "DONE! l = " << l << endl;
// 				// decodedEachIt[maxIts]++;
// 				// break;
// 			// }
// 			// decodedEachIt[l]++;
// 			unsigned int tmpSymbErrs = *symbolErrorsRemainingIt;
// 			unsigned int tmpFrameErrs = *frameErrorsRemainingIt;
// 			while (symbolErrorsRemainingIt < CurrentCWsymbolErrorsRemaining.end()){
// 				*symbolErrorsRemainingIt = tmpSymbErrs;
// 				*frameErrorsRemainingIt = tmpFrameErrs;
// 				++symbolErrorsRemainingIt;
// 				++frameErrorsRemainingIt;
// 			}
// 			break;
// 		}
// 		// else {
// 			// cout << "\t\t\tl = " << l << endl;
// 		// }
// 	}

// 	numErrs = erasuresRemaining;

// 	/* If there are still errors remaining after the final iteration
// 	 * increment numFrameFailures
// 	 */
// 	if (erasuresRemaining > 0){
// 		decodedEachIt[maxIts+1]++;
// 		numFrameFailures++;
// 	}
	
// 	// cout << "Erasures remaining after " << l+1 << " iterations: " << erasuresRemaining << endl;
// 	// cout << "elapsed_secs_CN = " << elapsed_secs_CN << endl;
// 	// cout << "elapsed_secs_CN_2 = " << elapsed_secs_CN_2 << endl;
// 	// cout << "elapsed_secs_CN_3 = " << elapsed_secs_CN_3 << endl;
// 	// cout << "elapsed_secs_CN_4 = " << elapsed_secs_CN_4 << endl;
// 	// cout << "elapsed_secs_CN_5 = " << elapsed_secs_CN_5 << endl;
// 	// cout << "elapsed_secs_VN = " << elapsed_secs_VN << endl;


// 	clock_t decode_end = clock();


// 	elapsed_secs_decode += double(decode_end - decode_begin) / CLOCKS_PER_SEC;
// 	numFramesDecoded++;	
// 	// if (l == maxIts){
// 		// decodedEachIt++;
// 	// }
// 	// // decodedEachIt[l]++;
// 	// cout << "elapsed_secs_decode = " << elapsed_secs_decode << endl;
// 	// cout << "average decode time (" << numFramesDecoded << " decodes so far) = " << elapsed_secs_decode / (double)numFramesDecoded << endl;
// 	// cout << "Its used: " << endl;
// 	// for (int it = 0; it <= maxIts; ++it){
// 	// 	cout << "\t" << it;
// 	// }
// 	// cout << endl;
// 	// for (vector<unsigned int>::iterator it = decodedEachIt.begin(); it < decodedEachIt.end(); ++it){
// 	// 	cout << "\t" << *it;
// 	// }
// 	// cout << endl;
// 	// for (vector<unsigned int>::iterator it = decodedEachIt.begin(); it < decodedEachIt.end(); ++it){
// 	// 	cout << "\t" << 100 * (float)(*it) / (float)(numFramesDecoded);
// 	// }
// 	// cout << endl;

// 	symbolErrorsRemaining = symbolErrorsRemaining + CurrentCWsymbolErrorsRemaining;
// 	frameErrorsRemaining = frameErrorsRemaining + CurrentCWframeErrorsRemaining;

// 	return RX;	
// }

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
	numFrameFailures = 0;

	decodedEachIt = vector<unsigned int>(maxIts + 2, 0);
	symbolErrorsRemaining = vector<unsigned int>(maxIts + 1, 0);
	frameErrorsRemaining = vector<unsigned int>(maxIts + 1, 0);
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

unsigned int BECDecoder::numFailures(){
	return numFrameFailures;
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

BECDecoder::BECDecoder(Eigen::SparseMatrix<bool,ColMajor> edgeList_in, const std::vector<unsigned int> &VNDegrees_in, const std::vector<unsigned int> &CNDegrees_in, const unsigned int i){
	edgeListColMajorSp = edgeList_in;
	edgeListRowMajorSp = SparseMatrix<bool,RowMajor>(edgeListColMajorSp);
	maxIts = i;

	numCNs = edgeListColMajorSp.rows();
	numVNs = edgeListColMajorSp.cols();

	VNDegrees = new unsigned int[numVNs];
	CNDegrees = new unsigned int[numCNs];

	unsigned int *VNDegrees_tmp = new unsigned int[numVNs];
	unsigned int *CNDegrees_tmp = new unsigned int[numCNs];

	maxVNDegree = VNDegrees_in[0];
	minVNDegree = VNDegrees_in[0];
	maxCNDegree = CNDegrees_in[0];
	minCNDegree = CNDegrees_in[0];	

	for (unsigned int i = 0; i < numVNs; ++i){
		unsigned int deg = VNDegrees_in[i];
		VNDegrees[i] = deg;
		VNDegrees_tmp[i] = 0;
		maxVNDegree = (deg > maxVNDegree) ? deg : maxVNDegree;
		minVNDegree = (deg < minVNDegree) ? deg : minVNDegree;		
	}
	for (unsigned int i = 0; i < numCNs; ++i){
		unsigned int deg = VNDegrees_in[i];
		CNDegrees[i] = CNDegrees_in[i];
		CNDegrees_tmp[i] = 0;
		maxCNDegree = (deg > maxCNDegree) ? deg : maxCNDegree;
		minCNDegree = (deg < minCNDegree) ? deg : minCNDegree;		
	}

	Connectivity = new bool[numCNs * numVNs];
	RowConnectivity = new unsigned int[numCNs * maxCNDegree];
	ColConnectivity = new unsigned int[numVNs * maxVNDegree];

	int count = 0;
	for (int i = 0; i < numVNs; ++i){
		for (Eigen::SparseMatrix<bool,ColMajor>::InnerIterator it(edgeListColMajorSp, i); it; ++it){
			int j = it.row();
			Connectivity[numCNs * j + i] = it.value();

			/* Update RowConnectivity for CN j */
			RowConnectivity[maxCNDegree * j + CNDegrees_tmp[j]++] = i; 

			/* Update ColConnectivity for VN i */
			ColConnectivity[maxVNDegree * i + VNDegrees_tmp[i]++] = j;

			count++;
		}
	}

	delete [] VNDegrees_tmp;
	delete [] CNDegrees_tmp;


	reset();
}

// BECDecoder::BECDecoder(std::vector< std::vector<unsigned int> > edgeListRowMajor_in, std::vector<std::vector<unsigned int> > edgeListColMajor_in, const std::vector<unsigned int> &VNDegrees_in, const std::vector<unsigned int> &CNDegrees_in, const unsigned int i){
// 	edgeListRowMajor = edgeListRowMajor_in;
// 	edgeListColMajor = edgeListColMajor_in;
// 	// edgeListColMajor = edgeList_in;
// 	// edgeListRowMajor = SparseMatrix<bool,RowMajor>(edgeListColMajor);
// 	maxIts = i;

// 	numCNs = edgeListColMajor.size();
// 	numVNs = edgeListRowMajor.size();

// 	VNDegrees = new unsigned int[numVNs];
// 	CNDegrees = new unsigned int[numCNs];

// 	unsigned int *VNDegrees_tmp = new unsigned int[numVNs];
// 	unsigned int *CNDegrees_tmp = new unsigned int[numCNs];

// 	maxVNDegree = VNDegrees_in[0];
// 	minVNDegree = VNDegrees_in[0];
// 	maxCNDegree = CNDegrees_in[0];
// 	minCNDegree = CNDegrees_in[0];	

// 	for (unsigned int i = 0; i < numVNs; ++i){
// 		unsigned int deg = VNDegrees_in[i];
// 		VNDegrees[i] = deg;
// 		VNDegrees_tmp[i] = 0;
// 		maxVNDegree = (deg > maxVNDegree) ? deg : maxVNDegree;
// 		minVNDegree = (deg < minVNDegree) ? deg : minVNDegree;		
// 	}
// 	for (unsigned int i = 0; i < numCNs; ++i){
// 		unsigned int deg = VNDegrees_in[i];
// 		CNDegrees[i] = CNDegrees_in[i];
// 		CNDegrees_tmp[i] = 0;
// 		maxCNDegree = (deg > maxCNDegree) ? deg : maxCNDegree;
// 		minCNDegree = (deg < minCNDegree) ? deg : minCNDegree;		
// 	}

// 	Connectivity = new bool[numCNs * numVNs];
// 	RowConnectivity = new unsigned int[numCNs * maxCNDegree];
// 	ColConnectivity = new unsigned int[numVNs * maxVNDegree];

// 	for (int a = 0; a < numCNs * numVNs; ++a){
// 		Connectivity[a] = false;
// 	}

// 	for (std::vector< std::vector<unsigned int> >::iterator i = edgeListColMajor.begin(); i < edgeListColMajor.end(); ++i){
// 		std::vector<unsigned int> vec = *i;
// 		int i_val = std::distance(edgeListColMajor.begin(), i);
// 		for (std::vector<unsigned int>::iterator j = (*i).begin(); j < (*i).end(); ++j){
// 			Connectivity[numCNs * (*j) + i_val] = true;

// 			/* Update RowConnectivity for CN j */
// 			RowConnectivity[maxCNDegree * (*j) + CNDegrees_tmp[(*j)]++] = i_val; 

// 			/* Update ColConnectivity for VN i */
// 			ColConnectivity[maxVNDegree * i_val + VNDegrees_tmp[i_val]++] = (*j);			
// 		}
// 	}

// 	delete [] VNDegrees_tmp;
// 	delete [] CNDegrees_tmp;

// 	reset();
// }

BECDecoder::~BECDecoder(){
	cout << "BECDecoder destructor: deleting Connectivity" << endl;
	delete [] VNDegrees;
	delete [] CNDegrees;
	delete [] RowConnectivity;
	delete [] ColConnectivity;
	delete [] Connectivity;
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