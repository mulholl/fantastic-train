#include "main.hpp"

using namespace std;
using namespace Eigen;

int main(void){
	clock_t begin = clock();

	MatrixXi A;
	SparseMatrix<double> B;
	SparseMatrix<bool> C;
	string fn = "edge_list.txt";
	string fn_out = "testout.txt";
	Triplet<double> T = Triplet<double>(1, 2, 0.5);
	vector< Triplet<double> > V;

	unsigned int maxIts = 10;

	float erasure_prob = 0.4;

	int seed = 1;

	cout << "Version: " << FANTASTIC_TRAIN_VERSION_MAJOR << "." << FANTASTIC_TRAIN_VERSION_MINOR << endl << endl;

	// V.push_back(T);
	// T = Triplet<double>(2, 3, 1.1);
	// V.push_back(T);
	// T = Triplet<double>(3, 2, 1.2);
	// V.push_back(T);
	// T = Triplet<double>(3, 3, 1.5);
	// V.push_back(T);

	// A.setZero(2, 2);
	// B.resize(4, 5);

	// B.setFromTriplets(V.begin(), V.end());

	// cout << "A = \n" << A << endl;
	// cout << "B = \n" << B << endl;

	// // cout << "B(0, 0) = " << B(0, 0) << endl;
	// // cout << "B(0, 0) = " << B(1, 2) << endl;
	// // cout << "B(0, 0) = " << B(2, 3) << endl;
	// // cout << "B(0, 0) = " << B(3, 2) << endl;
	// // cout << "B(0, 0) = " << B(3, 3) << endl;

	// cout << 	"B.outerSize() = " << B.outerSize() << endl;

	// for (int k=0; k<B.outerSize(); ++k){
	// 	for (SparseMatrix<double>::InnerIterator it(B,k); it; ++it){
	// 	cout << "it.value() = " << it.value() << endl;
	// 	cout << "it.row() = " << it.row() << endl;   // row index
	// 	cout << "it.col() = " << it.col() << endl;  // col index (here it is equal to k)
	// 	cout << "it.index() = " << it.index() << "\n" << endl; // inner index, here it is equal to it.row()
	// 	}
	// }

	// for (unsigned int i = 0; i < B.rows(); ++i){
	// 	for (unsigned int j = 0; j < B.cols(); ++j){
	// 		cout << B.coeffRef(i, j) << " ";
	// 	}
	// 	cout << endl;
	// }

	cout << "\n\n\n\n2. Trying to load the sparse matrix in the file " << fn << "... " << flush;	

	loadSparseBinMatFromTxt(C, fn);

	// cout << "2. C = " << endl << C << endl;

	// cout << "\n\n\n\n3. Trying to save the sparse matrix to the file " << fn_out << "... " << flush;

	// saveSparseBinMatToTxt(C, fn_out);

	// cout << "DONE" << endl;

	// cout << "\n\n\n\n4. Trying to read the matrix in the file " << "dense.txt" << "... " << flush;

	// MatrixXi D;

	// readMatFromTxt(D, "dense.txt");

	// cout << "DONE" << endl;

	std::mt19937 RNG(seed);

	unsigned int block_length = C.cols();

	cout << "There are " << C.cols() << " VNs and " << C.rows() << " CNs." << endl;

	BEChannel BEC(erasure_prob, RNG);

	// cout << "The Mersenne twister RNG, seeded with " << seed << ", gives " << RNG() << " " << RNG() << " " << RNG() << " " << RNG() << " " << endl;

	/* Generate an all-zero codeword */
	BinTXVector codeword(block_length, 0);
	/* Modulate the codeword using BPSK */
	BPSKTXVector modulatedCW = BPSKMod(codeword);
	/* Transmit the modulated codeword over the channel */
	BECBPSKRXVector ModulatedRXCW = BEC.useChannel(modulatedCW);

	cout << "\n\nThere were " << BEC.numErrors() << " erasures.\n" << endl;


	BECDecoder dec(C, maxIts);

	unsigned int numErrs;
	unsigned int totalErrs = 0;
	unsigned int totalBits = 0;
	unsigned int bitsSent;

	for (unsigned int i = 0; i < 1000; ++i){
		BECBPSKRXVector ModulatedRXCW = BEC.useChannel(modulatedCW);	
		BECRXVector RXCW = BPSKDemod(ModulatedRXCW);
		dec.decode(RXCW, modulatedCW, numErrs);
		bitsSent = modulatedCW.size();
		totalErrs += numErrs;
		totalBits += bitsSent;
	}

	cout << "totalErrs = " << totalErrs << endl;
	cout << "totalBits = " << totalBits << endl;
	cout << "BER = " << (float)totalErrs/(float)totalBits << endl;


	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "Elapsed time: " << elapsed_secs << endl;

	// cout << "\n\nQuitting\n\n" << endl;

	// cout << "B.Values = \n" << B.Values << endl;

	return 0;
}