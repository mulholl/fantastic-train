#include "BECDecoder.hpp"

using namespace std;
using namespace Eigen;

double elapsed_secs_decode = 0;
int total_decodes = 0;
	double elapsed_secs_CN = 0.0;
	double elapsed_secs_CN_2 = 0.0;
	double elapsed_secs_CN_3 = 0.0;
	double elapsed_secs_CN_4 = 0.0;
	double elapsed_secs_CN_5 = 0.0;
	double elapsed_secs_VN = 0.0;

vector<int> timesMaxItsUsed(11, 0);

BPSKRXVector BECDecoder::decode(const BECRXVector &RX_in, const BPSKTXVector &TX, unsigned int &numErrs){
	clock_t decode_begin = clock();
	BPSKRXVector RX;

	unsigned int break_count = 0;

	// set<float>::iterator pair_it;
	// bool pair_bool;
 //  	std::pair<std::set< vector<float> >::iterator,bool> ret;
	// ret = LUT1.insert(RX_in);
	// if (ret.second){
	// 	cout << "\t\t\tLUT1 returned TRUE (" << ret.second << ")" << endl;
	// }
	// else {
	// 	cout << "\tLUT1 returned FALSE" << endl;
	// 	exit(0);	
	// }

	// LUT1.insert(RX_in);

	// SparseMatrix<float> EdgeMessages;
	// EdgeMessages.resize(edgeListColMajor.rows(), edgeListColMajor.cols());

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

	// cout << "INITIAL ERASURESREMAINING = " << erasuresRemaining << endl;
	// cout << "or " << 100 * (float)erasuresRemaining / (float)RX_in.size() << "\%" << endl;

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

	unsigned int l;

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

			end_CN_4 = clock();

			elapsed_secs_CN_4 += double(end_CN_4 - begin_CN_4) / CLOCKS_PER_SEC;

			// unsigned int Bj_ind;
			// unsigned int i, idash;

			begin_CN_2 = clock();

			// cout << "Bj (using iterator i) = \n";
			// for (vector<unsigned int>::iterator i = Bj.begin(); i < Bj.end(); ++i){
			// 	cout << "\t" << *i;
			// }
			// cout << endl;

			for (vector<unsigned int>::iterator i = Bj.begin(); i < Bj.end(); ++i){

				// clock_t begin_CN_2 = clock();	

			// for (Bj_ind = 0; Bj_ind < Bj.size(); ++Bj_ind){
				// i = Bj[Bj_ind];

				/* Check whether all messages into CN j other than the one coming from
				 * VN i are known
				 */

				bool allKnown = true;
				unsigned int idash;
				float sum = 0;

				bool do_exit = false;

				unsigned int counter = 0;
				unsigned int falseCount = 0;

				// cout << "j = " << j << endl;

				// cout << "Bj.size() = " << Bj.size() << endl;
				for (vector<unsigned int>::iterator it = Bj.begin(); it < Bj.end(); ++it){
					idash = *it;
					vector<float>::iterator M_it = M.begin() + idash;
					// cout << "i = " << *i << " | idash = " << idash << endl;
					if (idash == *i){
						// cout << "i = " << *i << " | M_it = " << *M_it << endl;
						// do_exit = true;
						continue;
					}
					else if (*M_it == 0.5){
						// cout << "idash = " << idash << " | M_it = " << *M_it << endl;
						// cout << "SETTING allKnown = FALSE" << endl;
						allKnown = false;
						falseCount++;
						break;
					}
					else {
						// cout << "idash = " << idash << " | M_it = " << *M_it << endl;
						sum = fmod(sum + *M_it, 2);
						counter++;
					}
				}

				// if (allKnown){
				// 	cout << "falseCount = " << falseCount << endl;
				// 	exit(0);	
				// }

				// if (j == 2943 && *i == 89){
				// 	cout << "j = 2943, *i = 89 | allKnown = " << allKnown << endl;
				// 	cout << "l = " << l << endl;	
				// 	count_ji++;
				// 	cout << "count_ji = " << count_ji << endl;
				// 	cout << "EdgeMessages.coeffRef(j, *i) = " << EdgeMessages.coeffRef(j, *i) << endl;
				// }

				if (allKnown) {
					// SparseMatrix<bool, RowMajor>::InnerIterator it(edgeListRowMajor, j);
					// it = it + *i;
					// (*it) = sum;
					// vec.push_back(Triplet<float>(j, *i, sum));

					vec.push_back(Triplet<float>(j, *i, 100.00));
					// notpointfives++;

					// vec.push_back(Triplet<bool>(rowInd, colInd, true));
					// EdgeMessages.coeffRef(j, *i) = sum;
					// cout << "sum = " << sum << endl;
				}
				else {
					vec.push_back(Triplet<float>(j, *i, 0.5));
					// pointfives++;
					// EdgeMessages.coeffRef(j, *i) = 0.5;
				}
			}

			end_CN_2 = clock();
			elapsed_secs_CN_2 += double(end_CN_2 - begin_CN_2) / CLOCKS_PER_SEC;
		}

		// cout << "pointfives = " << pointfives << endl;
		// cout << "notpointfives = " << notpointfives << endl;

		begin_CN_3 = clock();

		// cout << "EdgeMessages.rows() = " << EdgeMessages.rows() << endl;
		// cout << "EdgeMessages.cols() = " << EdgeMessages.cols() << endl;

		// SparseMatrix<float> tmpSparse;

		// tmpSparse.resize(0, 0);
		// tmpSparse.resize(edgeListColMajor.rows(), edgeListColMajor.cols());

		EdgeMessages.setFromTriplets(vec.begin(), vec.end());


		// 			cout << "tmpSparse.coeffRef(2943, 89) = " << tmpSparse.coeffRef(2943, 89) << endl;		

		// EdgeMessages = tmpSparse;


		// 			cout << "EdgeMessages.coeffRef(2943, 89) = " << EdgeMessages.coeffRef(2943, 89) << endl;		

		// cout << "EdgeMessages.rows() = " << EdgeMessages.rows() << endl;
		// cout << "EdgeMessages.cols() = " << EdgeMessages.cols() << endl;
		

		end_CN_3 = clock();

		end_CN = clock();

		elapsed_secs_CN += double(end_CN - begin_CN) / CLOCKS_PER_SEC;	
		elapsed_secs_CN_2 += double(end_CN_2 - begin_CN_2) / CLOCKS_PER_SEC;
		elapsed_secs_CN_3 += double(end_CN_3 - begin_CN_3) / CLOCKS_PER_SEC;	
		elapsed_secs_CN_4 += double(end_CN_4 - begin_CN_4) / CLOCKS_PER_SEC;				

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
				// cout << "edgeListColMajor.innerSize() = " << edgeListColMajor.innerSize() << endl;
				// cout << "edgeListColMajor.outerSize() = " << edgeListColMajor.outerSize() << endl;
				// cout << "edgeListColMajor.rows() = " << edgeListColMajor.rows() << endl;
				// cout << "edgeListColMajor.cols() = " << edgeListColMajor.cols() << endl;
				// cout << "EdgeMessages.innerSize() = " << EdgeMessages.innerSize() << endl;
				// cout << "EdgeMessages.outerSize() = " << EdgeMessages.outerSize() << endl;
				// cout << "EdgeMessages.rows() = " << EdgeMessages.rows() << endl;
				// cout << "EdgeMessages.cols() = " << EdgeMessages.cols() << endl;
				float Eji = EdgeMessages.coeffRef(j, *i); // The message on the edge connected to CN j

				// cout << "edgeListColMajor.coeffRef(j, *i) = " << edgeListColMajor.coeffRef(j, *i) << endl;

				if (!edgeListColMajor.coeffRef(j, *i)){
					cout << "PROBLEM" << endl;
					exit(0);
				}

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

				if (Eji != 100 && Eji != 0.5){
					cout << "j = " << j << " | *i = " << *i << endl;
					cout << "Eji = " << Eji << endl;
					cout << "EdgeMessages.rows() = " << EdgeMessages.rows() << " | EdgeMessages.cols() = " << EdgeMessages.cols() << endl;
					cout << "EdgeMessages.coeffRef(" << j << ", " << *i << ") = " << EdgeMessages.coeffRef(j, *i) << endl;
					cout << "PROBLEM 2" << endl;
					exit(0);
				}

				// exit(0);
			}

			counter++;

			if (!madeCorrection){
				i++;
			}
		}

		// cout << "Iteration #" << l+1 << ": went through " << counter << " of " << tmp << " erasures." << endl;

		clock_t end_VN = clock();	

		elapsed_secs_VN += double(end_VN - begin_VN) / CLOCKS_PER_SEC;				


		// cout << "count = " << count << endl;
		// exit(0);

		// cout << "After iteration #" << l+1 << ", " << erasuresRemaining << " errors remain." << endl;

		/* Stopping criterion - Johnson, Algorithm 2.1 lines 27-28 
		 * (erasuresRemaining == 0 is the same as "all M_i known")
		 */
		if (erasuresRemaining == 0){
			// cout << "erasuresRemaining = " << erasuresRemaining << endl;
			// cout << "erasuresCorrected = " << erasuresCorrected << endl;
			// cout << "DONE! l = " << l << endl;
			timesMaxItsUsed[l]++;
			break;
		}
		else if (!correctionsThisIt){
			// break_count++;
			// if (break_count == 5){
				cout << "Breaking due to no corrections made" << endl;
				// cout << "DONE! l = " << l << endl;
				// timesMaxItsUsed[l-1]++;
				// break;
			// }
			timesMaxItsUsed[l]++;
			break;
		}
		// else {
			// cout << "\t\t\tl = " << l << endl;
		// }
	}

	numErrs = erasuresRemaining;
	
	// cout << "Erasures remaining after " << l+1 << " iterations: " << erasuresRemaining << endl;
	// cout << "elapsed_secs_CN = " << elapsed_secs_CN << endl;
	// cout << "elapsed_secs_CN_2 = " << elapsed_secs_CN_2 << endl;
	// cout << "elapsed_secs_CN_3 = " << elapsed_secs_CN_3 << endl;
	// cout << "elapsed_secs_CN_4 = " << elapsed_secs_CN_4 << endl;
	// cout << "elapsed_secs_CN_5 = " << elapsed_secs_CN_5 << endl;
	// cout << "elapsed_secs_VN = " << elapsed_secs_VN << endl;


	clock_t decode_end = clock();


	elapsed_secs_decode += double(decode_end - decode_begin) / CLOCKS_PER_SEC;
	total_decodes++;	
	// if (l == maxIts){
		// timesMaxItsUsed++;
	// }
	// timesMaxItsUsed[l]++;
	cout << "elapsed_secs_decode = " << elapsed_secs_decode << endl;
	cout << "average decode time (" << total_decodes << " decodes so far) = " << elapsed_secs_decode / (double)total_decodes << endl;
	cout << "Its used: " << endl;
	for (int it = 0; it < maxIts; ++it){
		cout << "\t" << it;
	}
	cout << endl;
	for (vector<int>::iterator it = timesMaxItsUsed.begin(); it < timesMaxItsUsed.end(); ++it){
		cout << "\t" << *it;
	}
	cout << endl;
	for (vector<int>::iterator it = timesMaxItsUsed.begin(); it < timesMaxItsUsed.end(); ++it){
		cout << "\t" << 100 * (float)(*it) / (float)(total_decodes);
	}
	cout << endl;

	// if (total_decodes == 50){
	// 	exit(0);
	// }

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