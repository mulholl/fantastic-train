#ifndef FANTASTIC_TRAIN_EXCEPTIONS_HPP
#define FANTASTIC_TRAIN_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace fantastic_train{
	class ChannelParamMinGTMax{
		public:
			ChannelParamMinGTMax(const float min_in, const float max_in) : min(min_in), max(max_in){};
			virtual const char* what() const throw(){
				std::string msg = "The minimum channel parameter specified (" + std::to_string(min) + ") is greater than the maximum (" + std::to_string(max) + ")\n";
				return msg.c_str();
			}	
		private:
			const float min, max;
	};
}

#endif