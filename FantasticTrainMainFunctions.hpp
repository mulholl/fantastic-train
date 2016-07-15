#ifndef FANTASTIC_TRAIN_MAIN_FUNCTIONS_HPP
#define FANTASTIC_TRAIN_MAIN_FUNCTIONS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <sstream>

template <typename T>
std::string printVector(const std::vector<T> &v, const std::string str){
	std::ostringstream ss;

	for (typename std::vector<T>::const_iterator it = v.begin(); it < v.end(); ++it){
		ss << str << *it;
	}
	ss << '\n';

	return ss.str();
}

void printNumDecodedEachIt(const std::vector<unsigned int> &v1, const std::vector<float> &v2, const std::string str);
void printErrorRates(const std::vector<float> &SER, const std::vector<float> &FER, const std::string str);
std::vector<float> getChannelParamList(const float channel_param);
std::vector<float> getChannelParamList(const float min, const float step, const float max);
std::vector<float> getChannelParamList(const std::vector<float> &list_in);
std::string listChannelParams(const std::vector<float> &param_list);

#endif