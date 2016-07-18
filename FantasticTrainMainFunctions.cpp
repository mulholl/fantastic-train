#include "FantasticTrainMainFunctions.hpp"

void printNumDecodedEachIt(const std::vector<unsigned int> &v1, const std::vector<float> &v2, std::string str){
	unsigned int l1 = v1.size();
	unsigned int l2 = v2.size();

	unsigned int l = (l1 > l2) ? l1 : l2;

	std::ostringstream ss;

	ss << str << "Initial";
	for (unsigned int i = 1; i < l-1; ++i){
		ss << str << i;
	}
	ss << str << "Undecoded\n";

	ss << "#" << printVector(v1, str);
	ss << "\%" << printVector(v2, str);

	std::cout << "Number and percentage of codewords decoded in each iteration:\n" << ss.str() << std::endl;
}

void printErrorRates(const std::vector<float> &SER, const std::vector<float> &FER, const std::string str){
	const std::ios_base::fmtflags orig_flags = std::cout.flags(std::ios::floatfield); // Save the current flag that specifying how floating point values are formatted for output - we'll want to restore this value later

	const unsigned int l1 = SER.size();
	const unsigned int l2 = FER.size();

	const unsigned int l = (l1 > l2) ? l1 : l2;

	const unsigned int width = 15;

	std::cout << std::scientific; // Output using scientific notation

	/* Print a heading for the table */
	std::cout << std::setw(width) << "Iteration #";
	std::cout << std::setw(width) << "SER";
	std::cout << std::setw(width) << "FER\n";
	std::cout << "=======================================================\n";

	std::cout << std::setw(width) << "Initial" << std::setw(width) << *(SER.begin()) << std::setw(width) << *(FER.begin()) << std::endl;
	for (unsigned int i = 1; i < l; ++i){
		std::cout << std::setw(width) << i << std::setw(width) << *(SER.begin() + i) << std::setw(width) << *(FER.begin() + i) << std::endl;
	}
	std::cout << std::endl;
	std::cout.flags(orig_flags); // Restore the original format flag for floating point values
}

std::vector<float> getChannelParamList(const float channel_param){
	std::vector<float> param_list(1, channel_param);

	return param_list;
}

std::vector<float> getChannelParamList(const float min, const float step, const float max){
	unsigned int num_els = (unsigned int)((max - min)/step);
	std::vector<float> param_list;

	if ((float)num_els * step == (max - min)){
		num_els++;
		param_list.reserve(num_els);
		param_list.push_back(min);

		for (unsigned int i = 1; i < num_els; ++i){
			param_list.push_back(min + (step * i));
		}
	}
	else {
		num_els++;
		param_list.reserve(num_els);
		param_list.push_back(min);

		for (unsigned int i = 1; i < num_els; ++i){
			param_list.push_back(min + (step * i));
		}

		param_list.push_back(max);
	}

	return param_list;
}

std::vector<float> getChannelParamList(const std::vector<float> &list_in){
	std::vector<float> param_list;
	bool duplicate;

	for (std::vector<float>::const_iterator it1 = list_in.begin(); it1 < list_in.end(); ++it1){
		duplicate = false;
		for (std::vector<float>::iterator it2 = param_list.begin(); it2 < param_list.end(); ++it2){
			if (*it1 == *it2){
				duplicate = true;
				break;
			}
		}
		if (!duplicate){
			param_list.push_back(*it1);
		}
	}

	return param_list;
}

std::string listChannelParams(const std::vector<float> &param_list){
	std::ostringstream oss;
	std::string str;

	oss << "Channel parameters to be simulated: \n";

	for (std::vector<float>::const_iterator it = param_list.begin(); it < param_list.end(); ++it){
		oss << "\t" << (*it);
	}
	oss << "\n";

	str = oss.str();

	return str;
}

/* Sort a vector of channel parameters into the order that will give results as 
 * early as possible  
 */
bool sortChannelParams(std::vector<float> &param_list, const unsigned int channel_model, const bool override){
	// std::vector<float> sortedVector;

	if (override){
		return false;
	}

	if (channel_model == CHANNEL_TYPE_BEC){
		// sortedVector = 
		std::sort(param_list.begin(), param_list.end(), sortDesc);
		// sortedVector = std::vector<float>(param_list.rbegin(), param_list.rend());
		// param_list = sortedVector;
		return true;
	}
	else {
		return false;
	}
}

bool sortDesc(const float a, const float b){
	return (a > b);
}

std::ofstream prepareOutputTxtFile(const std::string str, const unsigned int maxIts){
	std::ofstream ofs(str, std::ofstream::out);
	std::ostringstream oss;
	std::string tmp_str;

	oss << std::setw(17) << "Channel Parameter";

	// for (unsigned int i = 0; i <= maxIts; ++i){
		tmp_str =  "BER (" + std::to_string(maxIts) + " its)";
		oss << std::setw(15) << tmp_str;
	// }

	// for (unsigned int i = 0; i <= maxIts; ++i){
		tmp_str =  "FER (" + std::to_string(maxIts) + " its)";
		oss << std::setw(15) << tmp_str;
	// }	

	oss << '\n';

	tmp_str = "";

	unsigned int len = oss.str().length();

	for (unsigned int i = 0; i < len-1; ++i){
		tmp_str += '=';
	}
	tmp_str += '\n';

	oss << tmp_str;

	if (ofs.is_open()){
		ofs << oss.str();
	}

	ofs.flush();

	return ofs;
}

std::ofstream prepareSERTxtFile(const std::string str, const unsigned int maxIts){
	std::ofstream ofs(str, std::ofstream::out);
	std::ostringstream oss;
	std::string tmp_str;

	oss << std::setw(17) << "Channel Parameter";

	for (unsigned int i = 0; i <= maxIts; ++i){
		tmp_str =  "SER - " + std::to_string(i) + " its";
		oss << std::setw(15) << tmp_str;
	}

	oss << '\n';

	tmp_str = "";

	unsigned int len = oss.str().length();

	for (unsigned int i = 0; i < len-1; ++i){
		tmp_str += '=';
	}
	tmp_str += '\n';

	oss << tmp_str;

	if (ofs.is_open()){
		ofs << oss.str();
	}

	ofs.flush();

	return ofs;
}

std::ofstream prepareFERTxtFile(const std::string str, const unsigned int maxIts){
	std::ofstream ofs(str, std::ofstream::out);
	std::ostringstream oss;
	std::string tmp_str;

	oss << std::setw(17) << "Channel Parameter";

	for (unsigned int i = 0; i <= maxIts; ++i){
		tmp_str =  "FER - " + std::to_string(i) + " its";
		oss << std::setw(15) << tmp_str;
	}

	oss << '\n';

	tmp_str = "";

	unsigned int len = oss.str().length();

	for (unsigned int i = 0; i < len-1; ++i){
		tmp_str += '=';
	}
	tmp_str += '\n';

	oss << tmp_str;

	if (ofs.is_open()){
		ofs << oss.str();
	}

	ofs.flush();

	return ofs;
}

void saveResultsToTxt(std::ofstream &ofs, const float channel_param, const std::vector<float> &SER, const std::vector<float> &FER){
	std::ostringstream oss;
	oss << std::scientific; // Output using scientific notation

	oss << std::setw(17) << channel_param;

	// for (std::vector<float>::const_iterator it = SER.begin(); it < SER.end(); ++it){
		oss << std::setw(15) << *(SER.end() - 1);
	// }
	// for (std::vector<float>::const_iterator it = FER.begin(); it < FER.end(); ++it){
		oss << std::setw(15) << *(FER.end() - 1);
	// }

	oss << '\n';

	ofs << oss.str();

	ofs.flush();
}

void saveERResultsToTxt(std::ofstream &ofs, const float channel_param, const std::vector<float> &ER){
	std::ostringstream oss;
	oss << std::scientific; // Output using scientific notation

	oss << std::setw(17) << channel_param;

	for (std::vector<float>::const_iterator it = ER.begin(); it < ER.end(); ++it){
		oss << std::setw(15) << *it;
	}

	oss << '\n';

	ofs << oss.str();

	ofs.flush();
}