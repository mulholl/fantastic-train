#include "BECDecoder.hpp"

using namespace std;
// using namespace Eigen;

double elapsed_secs_decode = 0;
double elapsed_secs_1 = 0.0;
double elapsed_secs_2 = 0.0;
double elapsed_secs_3 = 0.0;
double elapsed_secs_4 = 0.0;
double elapsed_secs_5 = 0.0;
double elapsed_secs_6 = 0.0;

BPSKRXVector BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs){
	float M[numVNs];
	unsigned int numRemainingErrors = 0;
	unsigned int numInitialErrors = 0;


	copy(RX_in.begin(), RX_in.end(), M);
	for (unsigned int i = 0; i < numVNs; ++i){
		if (M[i] == 0.5){
			numInitialErrors++;
		}
	}

	numRemainingErrors = numInitialErrors;
	numSymbolsDecoded+= numVNs;
	numFramesDecoded++;

	unsigned int errorsCorrectedThisIt = 0;

	unsigned int numCorrectedErrs = 0;

	float *EdgeMessages = new float[numCNs * numVNs];

	unsigned int i, i_dash, j, l;
	unsigned int i_it, j_it, i_dash_it;
	int startInd, endInd;

	bool allOtherMessagesIntoCNKnown; // TRUE if all of the incoming messages to a CN, other than the message on the currently-considered edge, are known
	bool mod2_sum; // Used for mod-2 addition

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
			EL->CNConnInds(j, startInd, endInd);
			for (i_it = startInd; i_it <= endInd; ++i_it){
				i = EL->CNConnections[i_it];

				allOtherMessagesIntoCNKnown = true;

				/* Johnson Algorithm 2.1 lines 11-15 */
				for (i_dash_it = startInd; i_dash_it <= endInd; ++i_dash_it){
					i_dash = EL->CNConnections[i_dash_it];

					if (i_dash == i){
						continue;
					}
					else if (M[i_dash] == 0.5) {
						allOtherMessagesIntoCNKnown = false;
					}
					else {
						mod2_sum = (M[i_dash] == 0) ? mod2_sum : (!mod2_sum);
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
				EL->VNConnInds(i, startInd, endInd);
				for (j_it = startInd; j_it <= endInd; ++j_it){
					j = EL->VNConnections[j_it];

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
		/* End Johnson Algorithm 2.1 Lines 19-25 */

		/* Johnson Algorithm 2.1 Lines 27-28 */
		if (numRemainingErrors == 0){
			decodedEachIt[l]++;
			break;
		}
		else {
			symbolErrorsRemaining[l] += numRemainingErrors;
			frameErrorsRemaining[l]++;
		}
		/* End Johnson Algorithm 2.1 Lines 27-28 */
		if (errorsCorrectedThisIt == 0){
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

BECDecoder::BECDecoder(const EdgeList * EL_in, const unsigned int &maxIts_in) : EL(EL_in){
	// EL = &EL_in;
	// EL = new EdgeList("edge_list.txt", "txt");
	maxIts = maxIts_in;

	numCNs = EL->CNs();
	numVNs = EL->VNs();

	VNDegrees = EL->VNDegrees;
	CNDegrees = EL->VNDegrees;

	maxVNDegree = EL->maxVNDegree();
	minVNDegree = EL->minVNDegree();

	maxCNDegree = EL->maxCNDegree();
	minCNDegree = EL->minCNDegree();

	reset();
}

BECDecoder::~BECDecoder(){
	VNDegrees = NULL;
	CNDegrees = NULL;
}