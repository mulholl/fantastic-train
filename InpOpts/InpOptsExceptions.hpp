#ifndef INP_ARGS_EXCEPTIONS_HPP
#define INP_ARGS_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <sstream>

namespace inpopts {
	struct  BadType{ };
	struct 	BadPrioritySpec{ };
	struct 	FileOpenFailed{ };
	class 	OptRedefined{
		public:
			OptRedefined(char c){
				optName = c + '\0';
			}
			OptRedefined(std::string str){
				optName = str;
			}
			virtual const char* what() const throw(){
				std::string msg = "The option " + optName + " was specified multiple times\n";
				return msg.c_str();
			}
		private:
			std::string optName;
	};
	class 	UnrecOpt{
		public:
			UnrecOpt(char c){
				optName = c + '\0';
			}
			UnrecOpt(std::string str){
				optName = str;
			}
			virtual const char* what() const throw(){
				std::string msg = "The option " + optName + " is not valid\n";
				return msg.c_str();
			}
		private:
			std::string optName;
	};
	class IncNumArgs{
		public:
			IncNumArgs(char c, int inp_min, int inp_max, int inp_actual){
				optName = c + '\0';
				min = inp_min;
				max = inp_max;
				actual = inp_actual;
			}
			IncNumArgs(std::string str, int inp_min, int inp_max, int inp_actual){
				optName = str;
				min = inp_min;
				max = inp_max;
				actual = inp_actual;
			}
			virtual const char* what() const throw(){
				std::string msg;
				std::string amt_provided;

				if (actual == 1){
					amt_provided = std::to_string(actual) + "argument was";
				}
				else {
					amt_provided = std::to_string(actual) + "arguments were";
				}
				if (actual < min){
					amt_provided = "only " + amt_provided;
				}

				if (min == max){
					if (max == 0){
						/* No arguments may be given */
						msg = "The option " + optName + " does not take any arguments, but " + amt_provided + " provided.\n";
					}
					else if (max > 0){
						/* An exact number of arguments is required */
						std::string amt_required;
						if (max == 1){
							amt_required = std::to_string(max) + "argument";
						}
						else {
							amt_required = std::to_string(max) + "arguments";
						}
						msg = "The option " + optName + " requires exactly " + amt_required + ", but " + amt_provided + " provided.\n";
					}
				}
				else if (min >= 0 && max < 0){
					/* A minimum number of arguments is required but there is no maximum */
					std::string amt_required;
					if (min == 1){
						amt_required = std::to_string(min) + "argument";
					}
					else {
						amt_required = std::to_string(min) + "arguments";
					}
					msg = "The option " + optName + " requires at least " + amt_required + ", but " + amt_provided + " provided.\n";
				}
				else if (max > 0 && min < 0){
					/* A maximum number of arguments is allowed but there is no minimum */
					std::string amt_required;
					if (max == 1){
						amt_required = std::to_string(max) + "argument";
					}
					else {
						amt_required = std::to_string(max) + "arguments";
					}
					msg = "The option " + optName + " can take up to " + amt_required + ", but " + amt_provided + " provided.\n";
				}
				else if (min >= 0 && max > 0){
					/* A minimum and maximum number of arguments are specified */
					std::string amt_required = std::to_string(min) + " and " + std::to_string(max) + " arguments";
					msg = "The option " + optName + " can take between " + amt_provided + ", but " + amt_provided + " provided.\n";
				}
				return msg.c_str();
			}
			int min, max, actual;
		private:
			std::string optName;
	};
	struct OutsideRange : std::exception {
		// public:		
			virtual const char* what() const throw(){
				std::string msg = "The specified value is outside the range of allowed values.\n";
				return msg.c_str();
			}		
	};

	class MultCmdSpec{
	public:
			MultCmdSpec(const char c, const std::string str){
				if (c == '\0'){
					optName = "--" + str;
				}
				else {
					std::stringstream ss;
					ss << "-" << c << '\0';
					optName = ss.str();
				}
			}
			virtual const char* what() const throw(){
				std::string msg = "The option " + optName + " was specified multiple times on the command line.\n";
				return msg.c_str();
			}
		private:
			std::string optName;
	};

	class MultFileSpec{
	public:
			MultFileSpec(const char c, const std::string str){
				if (c == '\0'){
					optName = "--" + str;
				}
				else {
					std::stringstream ss;
					ss << "-" << c << '\0';
					optName = ss.str();
				}
			}
			virtual const char* what() const throw(){
				std::string msg = "The option " + optName + " was specified multiple times in the input file.\n";
				return msg.c_str();
			}
		private:
			std::string optName;	
	};

	class UnrecOpts{
	public:
			UnrecOpts(const std::string str_in){
				str = str_in;
			}
			virtual const char* what() const throw(){
				return str.c_str();
			}
		private:
			std::string str;	
	};	

	class CannotConvert { };

	class VecInUse{ };
	class VecNotInUse{ };
	class NeedToUseVector{
		public:
			virtual const char* what() const throw(){
				std::string msg = "The arguments provided were valid but cannot be stored in a single variable as required.\n";
				return msg.c_str();
			}		
	};
	class BadBoolValue{
		public:
			virtual const char* what() const throw(){
				std::string msg = "Invalid argument passed for an option which takes only boolean arguments.\n";
				return msg.c_str();
			}	
	};
	class ExpectedChar{
		public:
			virtual const char* what() const throw(){
				std::string msg = "Invalid argument passed for an option which takes only char arguments.\n";
				return msg.c_str();
			}	
	};

	/* Exceptions for cases where the numbers of allowed arguments are not consistent */
	class MaxArgsLessThanMinArgs{
		public:
			virtual const char* what() const throw(){
				std::string msg = "The specified maximum number of arguments is less than the specified minimum number of arguments.\n";
				return msg.c_str();
			}
	};

	/* Exceptions for cases where the default value is not one of the specified acceptable values */
	class MaxLessThanDefault{
		public:
			virtual const char* what() const throw(){
				std::string msg = "The specified maximum value is less than the previously specified default value.\n";
				return msg.c_str();
			}
	};		
	class MinGreaterThanDefault{
		public:
			virtual const char* what() const throw(){
				std::string msg = "The specified minimum value is greater than the previously specified default value.\n";
				return msg.c_str();
			}
	};	
	class ListDoesNotContainDefault{
		public:
			virtual const char* what() const throw(){
				std::string msg = "The specified list of acceptable values does not contain the previously specified default value.\n";
				return msg.c_str();
			}
	};		
	class DefaultOutsideRange{
		public:
			virtual const char* what() const throw(){
				std::string msg = "The specified default value lies outside the previously specified range/list of valid values.\n";
				return msg.c_str();
			}
	};	
	struct LTMin : std::exception {
		// public:
			virtual const char* what() const throw(){
				std::string msg = "The specified value is less than the minimum allowed value.\n";
				return msg.c_str();
			}		
	};
	struct GTMax : std::exception {
		// public:		
			virtual const char* what() const throw(){
				std::string msg = "The specified value is greater than the maximum allowed value.\n";
				return msg.c_str();
			}				
	};
	struct NotInList : std::exception {
		// public:
			virtual const char* what() const throw(){
				std::string msg = "The specified value is not in the list of allowed values.\n";
				return msg.c_str();
			}		
	};
	class ValidationFailure{
	public:
			ValidationFailure(const std::string str_in) : str(str_in) {
				// str = str_in;
			}
			virtual const char* what() const throw(){
				return str.c_str();
			}
	private:
			std::string str;	
	};
}

#endif