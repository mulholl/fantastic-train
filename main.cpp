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
	float channel_param_min, channel_param_max, channel_param_step;
	vector<float> channel_param_range;
	vector<float> channel_param_list;
	string channel_model;

	vector<string> valid_channel_models;
	valid_channel_models.push_back("BEC");


	int seed;
	unsigned int max_decoding_failures;
	unsigned int max_frames;

	// InpOptsClass opts(argc, argv, config_filename, "cf");
	InpOptsClass opts(argc, argv, config_filename, "cf");

	opts.addOpt('h', "help", "Print this help message");

	unsigned int count = 0;

	// cout << "\n\naddOpt() for channel_param" << endl;
	try {
		opts.addOpt('p', "channel_param", "Channel parameter value to be used", value<float>(channel_param).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_min", "Minimum value for a range of channel parameters", value<float>(channel_param_min).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_max", "Maximum value for a range of channel parameters", value<float>(channel_param_max).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_step", "Step size for a range of channel parameters", value<float>(channel_param_step).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_range", "Range of channel parameters (usage: --channel_param_range <MINIMUM> <STEP SIZE> <MAXIMUM>", value<float>(channel_param_range).numArgs(3).setRange(0.0,1.0));
		opts.addOpt("channel_param_list", "List of channel parameters", value<float>(channel_param_list).minNumArgs(1).setRange(0.0,1.0));
		// cout << "\n\naddOpt() for maxIts" << endl;
		opts.addOpt('i', "max_iterations", "Maximum number of decoder iterations to be simulated per transmission", value<unsigned int>(maxIts).minNumArgs(0).defaultValue(200));
		// cout << "\n\naddOpt() for seed" << endl;
		opts.addOpt('s', "seed", "Seed for pseudorandom number generator", value<int>(seed).defaultValue(time(NULL)));
		opts.addOpt('c', "config_file", "Path to configuration file (not currently working due to limitations in sturdy-robot...)", value<string>(config_filename).defaultValue("config.txt"));
		opts.addOpt('e', "edge_list_file", "Path to file containing parity-check matrix edge list", value<string>(edge_list_filename).defaultValue("edge_list.txt"));
		opts.addOpt('d', "max_decoding_failures", "Maximum number of decoding failures per channel parameter value", value<unsigned int>(max_decoding_failures).defaultValue(100).numArgs(1));
		opts.addOpt('f', "max_frames", "Maximum number of frames to transmit per channel parameter value", value<unsigned int>(max_frames).defaultValue(1000000).numArgs(1));
		opts.addOpt('m', "channel_model", "Channel model to be used", value<string>(channel_model).numArgs(1).setValList(valid_channel_models));
	}
	/* Check if an exception was thrown when parsing the options, if so we print an error
	 * message and quit
	 */
	catch (const inpopts::ValidationFailure &e) {
		cout << "Error in input options: " << e.what() << endl;
		cout << "QUITTING!" << endl;
		return EXIT_BAD_INPUTS;
	}

	if (opts.Used("help") || opts.Used('h')){
		cout << "fantastic-train Version " << FANTASTIC_TRAIN_VERSION_MAJOR << "." << FANTASTIC_TRAIN_VERSION_MINOR << endl << endl;
		cout << "Options " << endl;
		cout << opts.listArgs() << endl;
		cout << "Makes use of sturdy-robot Version " << inpopts::Version() << endl;
	}

	unsigned int param_modes_used = 0;
	unsigned int param_mode, param_mode_single, param_mode_multi_line_range, param_mode_one_line_range, param_mode_list;
	string param_mode_string = "The channel parameter(s) were specified in multiple ways, please select one of the following: \n";
	if (opts.Used("channel_param")){
		param_modes_used++;
		param_mode_string = param_mode_string + to_string(param_modes_used) + ": Single channel parameter using the channel_param option\n";
		param_mode = CHANNEL_PARAM_MODE_SINGLE_VALUE;
		param_mode_single = param_modes_used;
	}
	if (opts.Used("channel_param_min") && opts.Used("channel_param_step") && opts.Used("channel_param_max")){
		param_modes_used++;
		param_mode_string = param_mode_string + to_string(param_modes_used) + ": Range of channel parameters using the channel_param_min, channel_param_max and channel_param_step options\n";
		param_mode = CHANNEL_PARAM_MULTI_LINE_RANGE_OF_VALUES;
		param_mode_multi_line_range = param_modes_used;
	}
	else if (opts.Used("channel_param_min") && opts.Used("channel_param_step") && opts.Used("channel_param_max")){

	}
	if (opts.Used("channel_param_range")){
		param_modes_used++;
		param_mode_string = param_mode_string + to_string(param_modes_used) + ": Range of channel parameters using the channel_param_range option\n";
		param_mode = CHANNEL_PARAM_ONE_LINE_RANGE_OF_VALUES;
		param_mode_one_line_range = param_modes_used;
	}
	if (opts.Used("channel_param_list")){
		param_modes_used++;
		param_mode_string = param_mode_string + to_string(param_modes_used) + ": List of channel parameters using the channel_param_list option\n";
		param_mode = CHANNEL_PARAM_LIST_OF_VALUES;
		param_mode_list = param_modes_used;
	}
	if (param_modes_used == 0){

	}
	else if (param_modes_used == 1 && param_mode == CHANNEL_PARAM_MULTI_LINE_RANGE_OF_VALUES){

	}
	else if (param_modes_used > 1){
		unsigned int tmp_ui;
		bool tmp_bool = true;
		cout << param_mode_string;
		while (tmp_bool){
			tmp_bool = false;
			cin >> tmp_ui;
			if (tmp_ui == param_mode_single){
				param_mode = CHANNEL_PARAM_MODE_SINGLE_VALUE;
			}
			else if (tmp_ui == param_mode_multi_line_range){
				param_mode = CHANNEL_PARAM_MULTI_LINE_RANGE_OF_VALUES;
			}
			else if (tmp_ui == param_mode_one_line_range){
				channel_param_min = channel_param_range[0];
				channel_param_step = channel_param_range[1];
				channel_param_max = channel_param_range[2];
				param_mode = CHANNEL_PARAM_ONE_LINE_RANGE_OF_VALUES;
			}
			else if (tmp_ui == param_mode_list){
				param_mode = CHANNEL_PARAM_LIST_OF_VALUES;
			}
			else {
				cout << "Invalid selection, please try again: ";
				tmp_bool = true;
			}
		}
		cout << "\n";
	}

	/* We create a vector containing all of the channel parameters we will use */
	vector<float> channel_params;

	if (param_mode == CHANNEL_PARAM_MODE_SINGLE_VALUE){
		channel_params = getChannelParamList(channel_param);
	}
	else if ((param_mode == CHANNEL_PARAM_MULTI_LINE_RANGE_OF_VALUES) || (param_mode == CHANNEL_PARAM_ONE_LINE_RANGE_OF_VALUES)){
		channel_params = getChannelParamList(channel_param_min, channel_param_step, channel_param_max);
	}
	else if (param_mode == CHANNEL_PARAM_LIST_OF_VALUES){
		channel_params = getChannelParamList(channel_param_list);
	}

	cout << listChannelParams(channel_params) << endl;

	cout << "channel_model = |" << channel_model << "|" << endl;

	exit(0);

	// cout << "channel_param = " << channel_param << endl;
	// cout << "channel_param_min = " << channel_param_min << endl;
	// cout << "channel_param_max = " << channel_param_max << endl;
	// cout << "channel_param_step = " << channel_param_step << endl;

	// cout << "\nchannel_param_range.size() = " << channel_param_range.size() << endl;
	// cout << "channel_param_range = " << endl;
	// for (vector<float>::iterator it = channel_param_range.begin(); it < channel_param_range.end(); ++it){
	// 	cout << "\t" << *it;
	// }
	// cout << endl << endl;

	// cout << "\nchannel_param_list.size() = " << channel_param_list.size() << endl;
	// cout << "channel_param_list = " << endl;
	// for (vector<float>::iterator it = channel_param_list.begin(); it < channel_param_list.end(); ++it){
	// 	cout << "\t" << *it;
	// }
	// cout << endl << endl;	

	cout << "maxIts = " << maxIts << endl;
	cout << "seed = " << seed << endl;
	cout << "max_decoding_failures = " << max_decoding_failures << endl;
	cout << "max_frames = " << max_frames << endl;
	// exit(0);

	cout << "\n\nLoading the edge list from the file " << edge_list_filename << " . . . " << flush;

	if (loadSparseBinMatFromTxt(C, edge_list_filename)){
		cout << "DONE!" << endl;
	}
	else {
		cout << "FAILED! Quitting . . ." << endl;
		return EXIT_FILE_FAILURE;
	}

	// vector< list<unsigned int> > rowMajor, colMajor;

	// loadSparseBinMatFromTxt(rowMajor, colMajor, edge_list_filename);

	/* Seed the RNG */
	std::mt19937 RNG(seed);

	unsigned int block_length = C.cols();

	cout << "\nThere are " << C.cols() << " VNs and " << C.rows() << " CNs." << endl;

	/* Create the channel object */
	BEChannel BEC(channel_param, RNG);

	/* Generate an all-zero codeword */
	BinTXVector codeword(block_length, 0);
	/* Modulate the codeword using BPSK */
	BPSKTXVector modulatedCW = BPSKMod(codeword);

	/* Create the decoder object */
	BECDecoder dec(C, maxIts);


	vector<float> SER, FER, percDecodedEachIt;
	vector<unsigned int> numDecodedEachIt;
	unsigned int numErrs;

	for (unsigned int i = 0; (i < max_decoding_failures) && (i < max_frames); ++i){
		/* Transmit the modulated codeword over the channel */
		BECBPSKRXVector ModulatedRXCW = BEC.useChannel(modulatedCW);	
		/* Demodulate the received vector */
		BECRXVector RXCW = BPSKDemod(ModulatedRXCW);
		/* Decode the received vector */
		dec.decode(RXCW, modulatedCW, numErrs);

		/* Update error statistics from the decoder */
		dec.SER(SER);
		dec.FER(FER);
		dec.codewordsDecodedEachIt(numDecodedEachIt, percDecodedEachIt);

		printErrorRates(SER, FER, "\t");
		printNumDecodedEachIt(numDecodedEachIt, percDecodedEachIt, "\t");	
	}


	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "Elapsed time: " << elapsed_secs << endl;

	// cout << "\n\nQuitting\n\n" << endl;

	// cout << "B.Values = \n" << B.Values << endl;

	return 0;
}