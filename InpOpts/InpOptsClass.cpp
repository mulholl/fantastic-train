#include "InpOptsClass.hpp"

using namespace std;

namespace inpopts{
	bool InpOptsClass::isCmdOption(const string str, bool &ls){
		unsigned int str_len = str.length();
		ls = false;
		
		// if (str_len > 1){
		// 	if (str[0] == '-' && str[1] != '-'){
		// 		return true;
		// 	}
		// 	else {
		// 		return false;
		// 	}
		// }
		// if (str_len > 2) {
		// 	if (str[0] == '-' && str[1] == '-'){
		// 		ls = true;
		// 		return true;
		// 	}
		// 	else {
		// 		return false;
		// 	}
		// }

		if (str_len > 1){
			if (str[0] == '-' && str[1] != '-'){
				return true;
			}
			else if (str_len > 2){
				ls = true;
				return true;
			}
			else{
				return false;
			}
		}
		// }
		// if (str_len > 2) {
		// 	if (str[0] == '-' && str[1] == '-'){
		// 		ls = true;
		// 		return true;
		// 	}
		// 	else {
		// 		return false;
		// 	}
		// }

		return false;
	}

	bool InpOptsClass::isCmdOption(const string str){
		bool temp_bool;

		return isCmdOption(str, temp_bool);
	}

	bool InpOptsClass::checkOpt(const char opt_short, const string opt_long, vector<string> &val, const bool updateCounters){

		vector<string> cmdVal;
		vector<string> fileVal;

		bool ret;

		// cout << "mode = " << mode <<endl;
		if (updateCounters){
			if (mode == INP_ARG_CMD_ONLY){
				if ((ret = checkCmdOpt(opt_short, opt_long, val))){
					if (ret == 1){
						++numRecognizedCmdOpts;
					}
					else {
						throw inpopts::MultCmdSpec(opt_short, opt_long);
					}
				}
			}
			else if (mode == INP_ARG_CMD_PRIORITY){
				if((ret = checkCmdOpt(opt_short, opt_long, val))){
					if (ret == 1){
						++numRecognizedCmdOpts;
					}
					else {
						throw inpopts::MultCmdSpec(opt_short, opt_long);
					}
				}
				else if ((ret = checkFileOpt(opt_short, opt_long, val))){
					if (ret == 1){
						++numRecognizedFileOpts;
					}
					else {
						throw inpopts::MultFileSpec(opt_short, opt_long);
					}
				}
			}
			else if (mode == INP_ARG_FILE_ONLY){
				if ((ret = checkFileOpt(opt_short, opt_long, val))){
					if (ret == 1){
						++numRecognizedFileOpts;
					}
					else {
						throw inpopts::MultFileSpec(opt_short, opt_long);
					}
				}
			}
			else if (mode == INP_ARG_FILE_PRIORITY){
				if ((ret = checkFileOpt(opt_short, opt_long, val))){
					if (ret == 1){
						++numRecognizedFileOpts;
					}
					else {
						throw inpopts::MultFileSpec(opt_short, opt_long);
					}
				}
				else if ((ret = checkCmdOpt(opt_short, opt_long, val))){
					if (ret == 1){
						++numRecognizedCmdOpts;
					}
					else {
						throw inpopts::MultCmdSpec(opt_short, opt_long);
					}
				}
			}
			else if (mode == INP_ARG_EQUAL_PRIORITY){

			}
		}
		else {
			if (mode == INP_ARG_CMD_ONLY){
				ret = checkCmdOpt(opt_short, opt_long, val);
			}
			else if (mode == INP_ARG_CMD_PRIORITY){
				ret = checkCmdOpt(opt_short, opt_long, val);
				if (!ret){
					ret = checkFileOpt(opt_short, opt_long, val);
				}
			}
			else if (mode == INP_ARG_FILE_ONLY){
				ret = checkFileOpt(opt_short, opt_long, val);
			}
			else if (mode == INP_ARG_FILE_PRIORITY){
				ret = checkFileOpt(opt_short, opt_long, val);
				if (!ret){
					ret = checkCmdOpt(opt_short, opt_long, val);
				}
			}
			else if (mode == INP_ARG_EQUAL_PRIORITY){

			}
		}

		return ret;
	}

	unsigned int InpOptsClass::checkCmdOpt(const char opt_short, const string opt_long, vector<string> &val){
		unsigned int count = 0;

		string opt_short_str(1, opt_short);

		vector<bool>::iterator it2 = recognizedCmdOpts.begin();
		vector<bool>::iterator it3 = cmdIsLong.begin();

		unsigned int l = 0;

		// for (vector<string>::iterator it1 = cmdOpts.begin(); it1 < cmdOpts.end(); ++it1){
			// cout << distance(cmdOpts.begin(), it1) << ": " << *it1 << endl;
		// }

		for (vector<string>::iterator it1 = cmdOpts.begin(); it1 < cmdOpts.end(); ++it1){

			it3 = cmdIsLong.begin() + distance(cmdOpts.begin(), it1); // *it3 will be a bool indicating whether the current option in *it1 was interpreted as a long (TRUE) or short (FALSE) option when it was stored

			if ((opt_short_str.compare(*it1) == 0 && !(*it3)) || (opt_long.compare(*it1) == 0 && (*it3))){
				val = *(cmdArgs.begin() + distance(cmdOpts.begin(), it1));
				*it2 = true;
				++count;
			}
			++it2;
		}

		return count;
	}

	unsigned int InpOptsClass::checkFileOpt(const char opt_short, const string opt_long, vector<string> &val){
		unsigned int count = 0;

		string opt_short_str(1, opt_short);

		vector<bool>::iterator it2 = recognizedFileOpts.begin();

		for (vector<string>::iterator it1 = fileOpts.begin(); it1 < fileOpts.end(); ++it1){
			if (opt_short_str.compare(*it1) == 0 || opt_long.compare(*it1) == 0){
				val = *(fileArgs.begin() + distance(fileOpts.begin(), it1));
				*it2 = true;
				++count;
			}
			++it2;
		}

		return count;
	}

	string InpOptsClass::listArgs(){
		string str = "";
		string opt_long = "";

		string argInfo;
		string valInfo;

		const int alignToS = 20;
		int alignToL = 0;;

		for (vector< tuple<char, string, string, int, bool, int, bool> >::iterator it = validArgList.begin(); it != validArgList.end(); ++it){
			int tmp2 = get<1>(*it).size();
			alignToL =  (tmp2 > alignToL) ? tmp2 : alignToL;
		}

		alignToL = alignToL + 15;

		// std::cout << "alignToL = " << alignToL << std::endl;

		for (vector< tuple<char, string, string, int, bool, int, bool> >::iterator it = validArgList.begin(); it != validArgList.end(); ++it){
			opt_long = "";
			opt_long = opt_long + "  -" + get<0>(*it) + "    --" + get<1>(*it);
			int tempSize = opt_long.size();

			for (int it = 0; it < alignToL - tempSize; ++it){
				opt_long = opt_long + " ";
			} 
			// std::cout << "opt_long = " << std::endl << opt_long << "|" << std::endl;

			opt_long = opt_long + get<2>(*it) + "\n";

			argInfo = "";

			for (int it = 0; it < alignToL; ++it){
				argInfo = argInfo + " ";
			} 

			if (get<4>(*it) && get<6>(*it)){
				if (get<3>(*it) == get<5>(*it)){
					argInfo = argInfo + "Takes exactly " + to_string(get<3>(*it)) + " arguments.\n";
				}
				else {
					argInfo = argInfo + "Takes " + to_string(get<3>(*it)) + "-" + to_string(get<5>(*it)) + " arguments.\n";
				}
			}
			else if (get<4>(*it)){
				argInfo = argInfo + "Takes at least " + to_string(get<3>(*it)) + " arguments.\n";
			}
			else if (get<6>(*it)){
				argInfo = argInfo + "Takes up to " + to_string(get<5>(*it)) + " arguments.\n";
			}
			else {
				argInfo = "";
			}

			str = str + opt_long + argInfo;
		}

		return str;
	}

	bool InpOptsClass::Used(const char c){
		vector<string> temp;
		return checkOpt(c, "", temp, false);
	}

	bool InpOptsClass::Used(const string str){
		vector<string> temp;
		const char c = '\0';
		return checkOpt(c, str, temp, false);
	}

	string InpOptsClass::Priority(){
		if (mode == INP_ARG_CMD_ONLY){
			return "CO";
		}
		else if (mode == INP_ARG_FILE_ONLY){
			return "FO";
		}
		else if (mode == INP_ARG_CMD_PRIORITY){
			return "CF";
		}
		else if (mode == INP_ARG_FILE_PRIORITY){
			return "FC";
		}
		else if (mode == INP_ARG_EQUAL_PRIORITY){
			return "EQ";
		}
		else {
			return "";
		}
	}

	void InpOptsClass::UnrecognizedOpts(string &str){
		bool cmd = false;
		bool file = false;
		string str1 = "";
		string str2 = "";
		string str3;
		vector<string>::iterator it2;
		vector<bool>::iterator it3;

		for (vector<bool>::iterator it1 = recognizedCmdOpts.begin(); it1 < recognizedCmdOpts.end(); ++it1){
			it2 = cmdOpts.begin() + distance(recognizedCmdOpts.begin(), it1);

			if (!*it1){
				cmd = true;
				it2 = cmdOpts.begin() + distance(recognizedCmdOpts.begin(), it1);
				str3 = *(it3 = cmdIsLong.begin() + distance(recognizedCmdOpts.begin(), it1)) ? "--" : "-";
				str1 = str1 + str3 + *it2 + " is not a reognized command-line option.\n";
			}
		}

		for (vector<bool>::iterator it1 = recognizedFileOpts.begin(); it1 < recognizedFileOpts.end(); ++it1){
			if (!*it1){
				file = true;
				it2 = fileOpts.begin() + distance(recognizedFileOpts.begin(), it1);
				str3 = *(it3 = cmdIsLong.begin() + distance(recognizedFileOpts.begin(), it1)) ? "--" : "-";
				str2 = str2 + str3 + *it2 + " is not a reognized input file option.\n";
			}
		}

		if (cmd && file){
			str = str1 + "\n" + str2;
		}
		else if (cmd){
			str = str1;
		}
		else if (file){
			str = str2;
		}

		if (cmd || file){
			throw inpopts::UnrecOpts(str);
		}

	}

	string InpOptsClass::UnrecognizedOpts(){
		string str;

		UnrecognizedOpts(str);

		return str;
	}
	
	std::string InpOptsClass::LastAttemptedAdd(){
		return lastAdded;
	}

	InpOptsClass::InpOptsClass(const int arg_c, char** arg_v){
		argc = arg_c;
		argv = arg_v;
		optionsFile.clear();
		validArgList.resize(0);

		numRecognizedCmdOpts = 0;
		numRecognizedFileOpts = 0;

		// getOptionsFile = false;

		mode = INP_ARG_CMD_ONLY;

		gatherCmdOpts();
		// printCmdOpts();

	}

	InpOptsClass::InpOptsClass(const int arg_c, char** arg_v, const string options_file, string priority){
		argc = arg_c;
		argv = arg_v;
		optionsFile = options_file;
		validArgList.clear();
		validArgList.resize(0);

		numRecognizedCmdOpts = 0;
		numRecognizedFileOpts = 0;

		priority = strToUpper(priority);

		// getOptionsFile = options_file.is_empty() ? true : false;

		if (priority.compare("CF") == 0){
			mode = INP_ARG_CMD_PRIORITY;
		}
		else if (priority.compare("FC") == 0){
			mode = INP_ARG_FILE_PRIORITY;
		}
		else if (priority.compare("EQ") == 0){
			mode = INP_ARG_EQUAL_PRIORITY;
		}
		else {
			throw inpopts::BadPrioritySpec{ };
		}

		gatherCmdOpts();
		// printCmdOpts();

		gatherFileOpts();
		// printFileOpts();

	}

	InpOptsClass::InpOptsClass(const string options_file){
		argc = 0;
		optionsFile = options_file;
		validArgList.clear();
		validArgList.resize(0);

		numRecognizedCmdOpts = 0;
		numRecognizedFileOpts = 0;

		// getOptionsFile = false;

		mode = INP_ARG_FILE_ONLY;

		// cout << "calling gatherFileOpts()" << endl;

		gatherFileOpts();

		// cout << "calling printFileOpts()" << endl;
		// printFileOpts();

	}


	/*	gatherCmdOpts() gathers the options provided at the command line and populates several vectors:
			cmdOpts, which contains the options
			cmdArgs, which, for each option, contains a vector of the arguments provided to the option
			optInds, which contains the indices in argv where the options appeared
			cmdNumArgs, which contains the number of arguments provided for each of these options
			cmdIsLong, which specifies whether the option was a long or a short option
		The validity of the options/arguments is not checked here. Each vector has identical length and corresponding indices.
	*/
	void InpOptsClass::gatherCmdOpts(){
		optInds.resize(0);
		cmdNumArgs.resize(0);
		cmdOpts.resize(0);
		cmdArgs.resize(0);
		recognizedCmdOpts.resize(0);
		cmdIsLong.resize(0);

		vector<string> temp_vec;

		string current;
		unsigned int current_len;
		bool temp_bool;

		for (unsigned int it = 1; it < argc; ++it){
			current = argv[it]; // Pick out the current input argument
			current_len = current.length();

			/* Check if this string is an option - i.e., if it begins with '-' */
			if (isCmdOption(current, temp_bool)){
				unsigned int i = temp_bool ? 2 : 1;
				// cout << "current = " << current << " | temp_bool = " << temp_bool << endl;
				current.erase(current.begin(), current.begin()+i);
				// cout << "current = " << current << endl;
				if (!temp_bool){
					for (string::iterator it2 = current.begin(); it2 < current.end(); ++it2){
						char current_char = *it2;
						string current_string = current.substr(distance(current.begin(), it2), 1);
						cmdIsLong.push_back(temp_bool);
						cmdOpts.push_back(current_string);
						recognizedCmdOpts.push_back(false);	
					}
				}
				else {
					cmdIsLong.push_back(temp_bool);
					cmdOpts.push_back(current);
					recognizedCmdOpts.push_back(false);
				}

				optInds.push_back(it);
			}
		}

		for (vector<unsigned int>::iterator it = optInds.begin(); it < optInds.end(); ++it){
			unsigned int current_opt_ind = *it;
			unsigned int next_opt_ind;
			temp_vec.clear();

			if (it < optInds.end() - 1){
				next_opt_ind = *(it + 1);
			}
			else {
				next_opt_ind = argc;
			}
			
			cmdNumArgs.push_back(next_opt_ind - current_opt_ind - 1);

			for (unsigned int it2 = current_opt_ind + 1; it2 < next_opt_ind; ++it2){
				temp_vec.push_back(argv[it2]);
			}

			cmdArgs.push_back(temp_vec);

		}
	}

	/* This is just for testing - it prints all of the options provided at the command line and the associated arguments, whether or not they are valid */
	void InpOptsClass::printCmdOpts(){
		cout << "Options/arguments provided at the command line:\n";
		cout << "\tOPTION\t\tARGUMENTS\n";
		for (unsigned int it = 0; it < cmdArgs.size(); ++it){
			// cout << "optInds[" << it << "] = " << optInds[it] << " | " << cmdNumArgs[it] << " arguments\n";
			cout << "\t" << cmdOpts[it] << "\n\t\t\t";
			for (vector<string>::iterator it2 = cmdArgs[it].begin(); it2 < cmdArgs[it].end(); ++it2){
				cout << *it2;
				if (it2 < cmdArgs[it].end() - 1){
					cout << ", ";
				}
			}
			cout << endl;
		}
	}

	// void InpOptsClass::convertStrToBool(vector<string> &argsAsStrings, vector<bool> &argsAsBools){
	// 	argsAsBools.resize(0);

	// 	string upper;

	// 	for (vector<string>::iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
	// 		upper = strToUpper(*it);
	// 		if (!upper.compare("Y") || !upper.compare("YES")  || !upper.compare("T") || !upper.compare("TRUE") || !upper.compare("ON")){
	// 			argsAsBools.push_back(true);
	// 		}
	// 		else if (!upper.compare("N") || !upper.compare("NO") || !upper.compare("F") || !upper.compare("FALSE") || !upper.compare("OFF")){
	// 			argsAsBools.push_back(false);
	// 		}
	// 		else {
	// 			throw inpopts::BadBoolValue{ };
	// 		}
	// 	}		
	// }

	// void InpOptsClass::convertStrToChar(vector<string> &argsAsStrings, vector<bool> &argsAsChars){
	// 	argsAsChars.resize(0);

	// 	for (vector<string>::iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
	// 		if ((*it).size() != 1){
	// 			throw ExpectedChar{ };
	// 		}
	// 		else {
	// 			argsAsChars.push_back((*it)[0]);
	// 		}
	// 	}
	// }

	// void convertStrToInt(vector<string> &argsAsStrings, vector<bool> &argsAsInts);
	// void convertStrToUInt(vector<string> &argsAsStrings, vector<bool> &argsAsUInts);
	// void convertStrToFloat(vector<string> &argsAsStrings, vector<bool> &argsAsFloats);
	// void convertStrToDouble(vector<string> &argsAsStrings, vector<bool> &argsAsDoubles);	


	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<string> &argsAsCorrectType){
		argsAsCorrectType = argsAsStrings;
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<char> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			if ((*it).size() != 1){
				throw ExpectedChar{ };
			}
			else {
				argsAsCorrectType.push_back((*it)[0]);
			}
		}
	}			

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<int> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		int val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stoi((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<long> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		long val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stol((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<long long> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		long long val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stoll((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<float> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		float val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stof((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<double> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		double val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stod((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}		

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<long double> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		long double val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stold((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<unsigned int> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		unsigned int val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stoul((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}			

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<unsigned long> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		unsigned long val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stoul((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<unsigned long long> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		unsigned long long val;

		string::size_type sz;   // alias of size_t

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			try {
				val = stoull((*it), &sz);
			}
			catch (exception e){
				cout << e.what() << endl;
			}
			argsAsCorrectType.push_back(val);
		}
	}	

	void InpOptsClass::convert(const vector<string> &argsAsStrings, vector<bool> &argsAsCorrectType){
		argsAsCorrectType.resize(0);

		string upper;

		for (vector<string>::const_iterator it = argsAsStrings.begin(); it < argsAsStrings.end(); ++it){
			upper = strToUpper(*it);
			if (!upper.compare("Y") || !upper.compare("YES")  || !upper.compare("T") || !upper.compare("TRUE") || !upper.compare("ON")){
				argsAsCorrectType.push_back(true);
			}
			else if (!upper.compare("N") || !upper.compare("NO") || !upper.compare("F") || !upper.compare("FALSE") || !upper.compare("OFF")){
				argsAsCorrectType.push_back(false);
			}
			else {
				throw inpopts::BadBoolValue{ };
			}
		}		
	}


	string InpOptsClass::strToUpper(const string str){
		string str2 = str;
		unsigned int len = str.length();

		for (unsigned int it = 0; it < len; ++it){
			str2[it] = toupper(str2[it]);
		}

		return str2;
	}

	/* trimLWSpace(string &str) trims any whitespace at the beginning of str */
	void InpOptsClass::trimLWSpace(string &str){
		locale loc;
		unsigned int str_len = str.length();
		unsigned int it;

		for (it = 0; it < str_len; ++it){
			if (!isspace(str[it], loc))
				break;
		}

		str = str.substr(it, str_len - it);
	}

	// /* trimLWSpace(string &str) trims any whitespace at the end of str */
	// void InpOptsClass::trimRWSpace(string &str){
	// 	locale loc;
	// 	unsigned int str_len = str.length();
	// 	int it;

	// 	// cout << "str = " << str << endl;

	// 	for (it = str_len - 1; it >= 0; --it){
	// 		// cout << "\tstr[" << it << "] = " << str[it] << endl;
	// 		if (!isspace(str[it], loc))
	// 			break;
	// 	}

	// 	str = str.substr(0, it + 1);

	// 	// cout << "str went from " << str_len << " to " << str.length() << " characters" << endl;
	// 	// cout << "str is now = " << str << endl;
	// }

	/* trimRWSpace(string &str) trims any whitespace at the end of str */
	void InpOptsClass::trimRWSpace(string &str){
		locale loc;
		unsigned int str_len = str.length();
		unsigned int it;

		if (str_len == 0){
			return;
		}

		for (it = str_len; it > 0; --it){
			if (!isspace(str[it-1], loc))
				break;
		}

		str = str.substr(0, it);
	}	

	/* splitAtFirstWs(string &str1) splits str1 in two at the first whitespace character. The return value is everything up to this character, str1 is modified to be everything after this character. Leading whitespace is trimmed from str1 before and after the split */
	string InpOptsClass::splitAtFirstWS(string &str1){
		locale loc;
		string str2 = "";

		trimLWSpace(str1);

		unsigned int str1_len = str1.length();

		for (unsigned int it = 0; it < str1_len + 1; ++it){
			if (it == str1_len){
				str2 = str1;
				str1 = "";
				break;
			}
			else if (isspace(str1[it], loc)){
				str2 = str1.substr(0, it);
				// cout << "it+1 = " << it+1 << endl;
				// cout << "str1_len = " << str1_len << endl;
				// cout << "str1_len - it - 1 = " << str1_len - it - 1 << endl;
				str1 = str1.substr(it+1, str1_len - it - 1);
				break;
			}
		}

		trimLWSpace(str1);

		return str2;
	}

	void InpOptsClass::gatherFileOpts(){
		bool header = false;
		// optInds.resize(0);
		fileNumArgs.resize(0);
		fileOpts.resize(0);
		fileArgs.resize(0);
		recognizedFileOpts.resize(0);
		fileIsLong.resize(0);

		vector<string> temp_vec;

		ifstream ifs(optionsFile);
		string current_line;
		string category = "";
		string option;
		string arguments, argument;
		unsigned int ind;

		if(!ifs.is_open()){
			throw inpopts::FileOpenFailed{ };
		}

		while(getline(ifs, current_line)){
			// cout << "current_line = \n" << current_line << endl;
			trimLWSpace(current_line);
			// cout << "current_line = \n" << current_line << endl;

			/* If this line is empty, move on to the next one */
			if (current_line.length() == 0){
				//cout << "Skipping this line as it's blank." << endl;
				continue;
			}
			/* If the line begins with '#', we assume it's a comment and move on to the next one */
			else if (current_line[0] == '#'){
				//cout << "Skipping this line as it's a comment." << endl;
				continue;
			}
			/* Check if this line begins a new category */
			else if (current_line[0] == '['){
				ind = current_line.find(']');
				category = current_line.substr(1, ind - 1);
				header = true;
				// cout << "category is [" << category << "]" << endl;
			}
			/* Otherwise, we consider this line to specify an option. We check for an equals sign and let everything before it (excluding any trailing whitespace) be the option */
			else {
				/* First, we serach for the equals sign */
				ind = current_line.find('=');

				/* We assume that everything that comes before this equals sign is the option, possibly with some whitespace at the end that we'll trim... */
				option = current_line.substr(0, ind);

				// cout << "option = " << "|" << option << "|" << endl;
				trimRWSpace(option);

				// cout << "option = " << "|" << option << "|" << endl;

				/* Now we store the category and option */
				if (header){
					fileOpts.push_back(category + '.' + option);
				}
				else {
					fileOpts.push_back(option);
				}
				recognizedFileOpts.push_back(false);

				/* If the option contains more than one character, consider it a long option */
				fileIsLong.push_back(option.length() > 1);

				/* We assume that everything that comes after the equals sign are the arugments for the option, separated by whitespace characters */
				arguments = current_line.substr(ind+1, current_line.length() - ind);
				trimLWSpace(arguments);

				temp_vec.resize(0);

				while (arguments.length()){
					argument = splitAtFirstWS(arguments);
					temp_vec.push_back(argument);
				}

				fileArgs.push_back(temp_vec);

				fileNumArgs.push_back(temp_vec.size());
			}
		}
	}

	void InpOptsClass::printFileOpts(){
		cout << "Options/arguments provided from the input file:\n";
		cout << "\tOPTION\t\tARGUMENTS\n";
		for (unsigned int it = 0; it < fileArgs.size(); ++it){
			// cout << "optInds[" << it << "] = " << optInds[it] << " | " << fileNumArgs[it] << " arguments\n";
			cout << "\t" << fileOpts[it] << "\n\t\t\t";
			for (vector<string>::iterator it2 = fileArgs[it].begin(); it2 < fileArgs[it].end(); ++it2){
				cout << "|" << *it2 << "|";
				if (it2 < fileArgs[it].end() - 1){
					cout << ", ";
				}
			}
			cout << endl;
		}
	}

}