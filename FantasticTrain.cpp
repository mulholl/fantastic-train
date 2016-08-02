#include "FantasticTrain.hpp"

using namespace std;
// using namespace Eigen;
using namespace inpopts;

int main(int argc, char *argv[]){
	clock_t begin = clock();

	EdgeList EL1;
	string edge_list_filename;
	string config_filename = "config.txt";

	unsigned int maxIts;

	float channel_param;
	float channel_param_min, channel_param_max, channel_param_step;
	vector<float> channel_param_range;
	vector<float> channel_param_list;
	string channel_model_str;
	unsigned int channel_model;
	string output_filename;

	vector<string> valid_channel_models;
	valid_channel_models.push_back("BEC");


	int seed;
	unsigned int max_decoding_failures;
	unsigned int max_frames;

	bool param_sort_override;

	// InpOptsClass opts(argc, argv, config_filename, "cf");
	InpOptsClass opts(argc, argv, config_filename, "cf");

	opts.addOpt('h', "help", "Print this help message");

	// cout << "\n\naddOpt() for channel_param" << endl;
	try {
		opts.addOpt('p', "channel_param", "Channel parameter value to be used", value<float>(channel_param).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_min", "Minimum value for a range of channel parameters", value<float>(channel_param_min).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_max", "Maximum value for a range of channel parameters", value<float>(channel_param_max).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_step", "Step size for a range of channel parameters", value<float>(channel_param_step).numArgs(1).setRange(0.0,1.0));
		opts.addOpt("channel_param_range", "Range of channel parameters (usage: --channel_param_range <MINIMUM> <STEP SIZE> <MAXIMUM>", value<float>(channel_param_range).numArgs(3).setRange(0.0,1.0));
		opts.addOpt("channel_param_list", "List of channel parameters", value<float>(channel_param_list).minNumArgs(1).setRange(0.0,1.0));
		opts.addOpt('i', "max_iterations", "Maximum number of decoder iterations to be simulated per transmission", value<unsigned int>(maxIts).minNumArgs(0).defaultValue(200));
		opts.addOpt('s', "seed", "Seed for pseudorandom number generator", value<int>(seed).defaultValue(time(NULL)));
		opts.addOpt('c', "config_file", "Path to configuration file (not currently working due to limitations in sturdy-robot...)", value<string>(config_filename).defaultValue("config.txt"));
		opts.addOpt('e', "edge_list_file", "Path to file containing parity-check matrix edge list", value<string>(edge_list_filename).defaultValue("edge_list.txt"));
		opts.addOpt('d', "max_decoding_failures", "Maximum number of decoding failures per channel parameter value", value<unsigned int>(max_decoding_failures).defaultValue(100).numArgs(1));
		opts.addOpt('f', "max_frames", "Maximum number of frames to transmit per channel parameter value", value<unsigned int>(max_frames).defaultValue(1000000).numArgs(1));
		opts.addOpt('m', "channel_model", "Channel model to be used", value<string>(channel_model_str).numArgs(1).setValList(valid_channel_models));
		opts.addOpt("param_sort_override", "Override channel parameter sorting", value<bool>(param_sort_override).numArgs(0).defaultValue(false));
		opts.addOpt('o', "output", "Path to output file", value<string>(output_filename).numArgs(1));
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
		cout << opts.listOpts() << endl;
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
	else if (param_modes_used == 1 && param_mode == CHANNEL_PARAM_ONE_LINE_RANGE_OF_VALUES){
				channel_param_min = channel_param_range[0];
				channel_param_step = channel_param_range[1];
				channel_param_max = channel_param_range[2];
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
	else if (param_mode == CHANNEL_PARAM_MULTI_LINE_RANGE_OF_VALUES){
		channel_params = getChannelParamList(channel_param_min, channel_param_step, channel_param_max);
	}
	else if (param_mode == CHANNEL_PARAM_ONE_LINE_RANGE_OF_VALUES){
		float channel_param_min_tmp = (channel_param_range[0] < channel_param_range[2]) ? channel_param_range[0] : channel_param_range[2];
		float channel_param_max_tmp = (channel_param_range[2] > channel_param_range[0]) ? channel_param_range[2] : channel_param_range[0];
		channel_params = getChannelParamList(channel_param_min_tmp, channel_param_range[1], channel_param_max_tmp);
	}
	else if (param_mode == CHANNEL_PARAM_LIST_OF_VALUES){
		channel_params = getChannelParamList(channel_param_list);
	}

	cout << setw(50) << "Maximum number of decoder iterations   -->   " << maxIts << endl;
	cout << setw(50) << "Seed for pseudorandom number generator   -->   " << seed << endl;
	cout << setw(50) << "Maximum number of decoding failures   -->   " << max_decoding_failures << '\n' << setw(50) << "per channel parameter value         " << endl;
	cout << setw(50) << "Maximum number of frames to send   -->   " << max_frames << '\n' << setw(50) << "per channel parameter value         " << endl;

	cout << "\n\nLoading the edge list from the file " << edge_list_filename << " . . . " << flush;

	unsigned int block_length;


	clock_t begin_file_read = clock();
	EL1.load(edge_list_filename, "txt");
	clock_t end_file_read = clock();

	cout << "\t\t\tIt took " << (double)(end_file_read - begin_file_read) / CLOCKS_PER_SEC << " secs to read the file using arrays!" << endl << endl;

	block_length = EL1.VNs();

	/* Seed the RNG */
	// std::mt19937 RNG(seed);
	srand(seed);

	unsigned int totalFramesSent = 0;


	/* Prepare the output file */
	std::ofstream txt_file_ofs = prepareOutputTxtFile(output_filename, maxIts);
	std::ofstream txt_file_ofs_BER = prepareSERTxtFile("output_BER.txt", maxIts);
	std::ofstream txt_file_ofs_FER = prepareFERTxtFile("output_FER.txt", maxIts);

	clock_t decode_begin, decode_end;
	double avg_decode_time;

	if (channel_model_str.compare("BEC") == 0){
		channel_model = CHANNEL_TYPE_BEC;

		cout << "Using BEC Simulator\n" << endl;

		/* We want to start with the least favorable channel parameter, as results in this
		 * region are obtained much more quickly, so we sort the vector of channel parameters
		 */
		sortChannelParams(channel_params, channel_model, param_sort_override);
		cout << listChannelParams(channel_params) << endl;

		/* Generate an all-zero codeword */
		BinTXVector codeword(block_length, 0);
		/* Modulate the codeword using BPSK */
		BPSKTXVector modulatedCW = BPSKMod(codeword);

		/* Create the decoder object */		
		BECDecoder dec1(&EL1, maxIts);

		vector<float> SER, FER, percDecodedEachIt;
		vector<unsigned int> numDecodedEachIt;
		unsigned int numErrs;

		decode_begin = clock();

		for (vector<float>::iterator channel_param_it = channel_params.begin(); channel_param_it < channel_params.end(); ++channel_param_it){
			float current_channel_param = *channel_param_it;

			/* Create the channel object */	
			// BEChannel BEC(current_channel_param, RNG);
			BEChannel BEC(current_channel_param);

			unsigned int frameErrors = 0;

			cout << "Current channel parameter: " << *channel_param_it << endl;

			for (unsigned int i = 0; i < max_frames; ++i){
				/* Transmit the modulated codeword over the channel */
				BECBPSKRXVector ModulatedRXCW = BEC.useChannel(modulatedCW);
				totalFramesSent++;
				/* Demodulate the received vector */
				BECRXVector RXCW = BPSKDemod(ModulatedRXCW);

				// frameErrors++;

				/* Decode the received vector */
				// dec1.decode(RXCW, modulatedCW, numErrs);
				/* Update error statistics from the decoder */
				dec1.SER(SER);
				dec1.FER(FER);
				dec1.codewordsDecodedEachIt(numDecodedEachIt, percDecodedEachIt);	

				/* If there are symbol errors remaining, increment the frameErrors counter */
				// frameErrors = numErrs ? (++frameErrors) : (frameErrors);

				// /* Update error statistics from the decoder */
				// dec.SER(SER);
				// dec.FER(FER);
				// dec.codewordsDecodedEachIt(numDecodedEachIt, percDecodedEachIt);

				// printErrorRates(SER, FER, "\t");
				// printNumDecodedEachIt(numDecodedEachIt, percDecodedEachIt, "\t");

				// decode_end = clock();
				// avg_decode_time = ((double)(decode_end - decode_begin) / CLOCKS_PER_SEC) / (float)(totalFramesSent);
				// std::cout << "Average decoder time per codeword: " << avg_decode_time << " seconds." << std::endl;

				if (frameErrors >= max_decoding_failures){
					break;
				}
			}

			/* At this point we have finished all of the simulations for this channel
			 * parameter, so we save the results.
			 */
			saveResultsToTxt(txt_file_ofs, current_channel_param, SER, FER);
			saveERResultsToTxt(txt_file_ofs_BER, current_channel_param, SER);
			saveERResultsToTxt(txt_file_ofs_FER, current_channel_param, FER);

			dec1.reset();
		}
	}

	cout << "Total frames sent: " << totalFramesSent << endl;

	/* Close the files we've opened */
	txt_file_ofs.close();
	txt_file_ofs_BER.close();
	txt_file_ofs_FER.close();

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	cout << "Elapsed time: " << elapsed_secs << endl;

	// cout << "\n\nQuitting\n\n" << endl;

	// cout << "B.Values = \n" << B.Values << endl;

	return 0;
}