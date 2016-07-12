#include "main.hpp"

using namespace std;
using namespace Eigen;
using namespace inpopts;

int main(int argc, char *argv[]){
	clock_t begin = clock();

	MatrixXi A;
	SparseMatrix<double> B;
	SparseMatrix<bool> C;
	string edge_list_filename;
	string config_filename = "config.txt";
	Triplet<double> T = Triplet<double>(1, 2, 0.5);
	vector< Triplet<double> > V;

	unsigned int maxIts;

	float channel_param;

	int seed;

	// InpOptsClass opts(argc, argv, config_filename, "cf");
	InpOptsClass opts(argc, argv, config_filename, "cf");

	opts.addOpt('h', "help", "Print this help message");
	opts.addOpt('p', "channel_param", "Channel parameter value to be used", value<float>(channel_param).setRange(0.0,1.0));
	opts.addOpt('i', "max_iterations", "Maximum number of decoder iterations to be simulated per transmission", value<unsigned int>(maxIts).minNumArgs(0).defaultValue(200));
	opts.addOpt('s', "seed", "Seed for pseudorandom number generator", value<int>(seed).defaultValue(time(NULL)));
	opts.addOpt('c', "config_file", "Path to configuration file (not currently working due to limitations in sturdy-robot...)", value<string>(config_filename).defaultValue("config.txt"));
	opts.addOpt('e', "edge_list_file", "Path to file containing parity-check matrix edge list", value<string>(edge_list_filename).defaultValue("edge_list.txt"));

	if (opts.Used("help") || opts.Used('h')){
		cout << "fantastic-train Version " << FANTASTIC_TRAIN_VERSION_MAJOR << "." << FANTASTIC_TRAIN_VERSION_MINOR << endl << endl;
		cout << "Options " << endl;
		cout << opts.listArgs() << endl;
		cout << "Makes use of sturdy-robot Version " << inpopts::Version() << endl;
	}

	cout << "channel_param = " << channel_param << endl;
	cout << "maxIts = " << maxIts << endl;
	cout << "seed = " << seed << endl;
	// exit(0);

	cout << "\n\n\n\n2. Trying to load the sparse matrix in the file " << edge_list_filename << "... " << flush;	

	loadSparseBinMatFromTxt(C, edge_list_filename);

	vector< list<unsigned int> > rowMajor, colMajor;

	loadSparseBinMatFromTxt(rowMajor, colMajor, edge_list_filename);

	for (vector< list<unsigned int> >::iterator it1 = rowMajor.begin(); it1 < rowMajor.begin() + 5; ++it1){
		for (list<unsigned int>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); ++it2){
			cout << *it2 << " " << flush;
		}
		cout << endl;
	}

	cout << endl << endl;

	for (vector< list<unsigned int> >::iterator it1 = colMajor.begin(); it1 < colMajor.begin() + 5; ++it1){
		for (list<unsigned int>::iterator it2 = (*it1).begin(); it2 != (*it1).end(); ++it2){
			cout << *it2 << " " << flush;
		}
		cout << endl;
	}

	cout << endl << endl;	

	// cout << "2. C = " << endl << C << endl;

	// cout << "\n\n\n\n3. Trying to save the sparse matrix to the file " << edge_list_filename_out << "... " << flush;

	// saveSparseBinMatToTxt(C, edge_list_filename_out);

	// cout << "DONE" << endl;

	// cout << "\n\n\n\n4. Trying to read the matrix in the file " << "dense.txt" << "... " << flush;

	// MatrixXi D;

	// readMatFromTxt(D, "dense.txt");

	// cout << "DONE" << endl;

	std::mt19937 RNG(seed);

	unsigned int block_length = C.cols();

	cout << "There are " << C.cols() << " VNs and " << C.rows() << " CNs." << endl;

	BEChannel BEC(channel_param, RNG);

	// cout << "The Mersenne twister RNG, seeded with " << seed << ", gives " << RNG() << " " << RNG() << " " << RNG() << " " << RNG() << " " << endl;

	/* Generate an all-zero codeword */
	BinTXVector codeword(block_length, 0);
	/* Modulate the codeword using BPSK */
	BPSKTXVector modulatedCW = BPSKMod(codeword);
	/* Transmit the modulated codeword over the channel */
	// BECBPSKRXVector ModulatedRXCW = BEC.useChannel(modulatedCW);

	// cout << "\n\nThere were " << BEC.numErrors() << " erasures.\n" << endl;

	BECDecoder dec(C, maxIts);

	unsigned int numErrs;
	unsigned int totalErrs = 0;
	unsigned int totalBits = 0;
	unsigned int frameErrs = 0;
	unsigned int totalFrames = 0;
	unsigned int bitsSent;


	vector<float> SER, FER, percDecodedEachIt;
	vector<unsigned int> numDecodedEachIt;


	// set< vector<float> > LUT;
	// set<unsigned int> LUT_errs;

	for (unsigned int i = 0; i < 100; ++i){
		totalFrames++;
		BECBPSKRXVector ModulatedRXCW = BEC.useChannel(modulatedCW);	
		BECRXVector RXCW = BPSKDemod(ModulatedRXCW);
		dec.decode(RXCW, modulatedCW, numErrs);
		bitsSent = modulatedCW.size();
		if (numErrs){
			frameErrs++;
		}
		totalErrs += numErrs;
		totalBits += bitsSent;

		cout << "After " << totalFrames << " codewords:" << endl;
		cout << "\ttotalErrs = " << totalErrs << endl;
		cout << "\ttotalBits = " << totalBits << endl;
		cout << "\ttotalFrames = " << totalFrames << endl;
		cout << "\tframeErrs = " << frameErrs << endl;
		cout << "\tBER = " << (float)totalErrs/(float)totalBits << endl;
		cout << "\tFER = " << (float)frameErrs/(float)totalFrames << endl;

		cout << "BEC.SER() = " << endl;
		dec.SER(SER);
		dec.FER(FER);
		dec.codewordsDecodedEachIt(numDecodedEachIt, percDecodedEachIt);

		for (vector<unsigned int>::iterator it = numDecodedEachIt.begin(); it < numDecodedEachIt.end(); ++it){
			cout << "\t" << *it;
		}
		cout << endl;
		for (vector<float>::iterator it = percDecodedEachIt.begin(); it < percDecodedEachIt.end(); ++it){
			cout << "\t" << *it;
		}
		cout << endl;		
	}


	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "Elapsed time: " << elapsed_secs << endl;

	// cout << "\n\nQuitting\n\n" << endl;

	// cout << "B.Values = \n" << B.Values << endl;

	return 0;
}