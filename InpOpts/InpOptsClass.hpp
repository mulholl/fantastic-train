#ifndef INP_ARGS_CLASS_HPP
#define INP_ARGS_CLASS_HPP

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <typeinfo> // for comparing types
#include <cctype>
#include <fstream>
#include <locale>
#include "InpOptsExceptions.hpp"
#include "InpOptsValue.hpp"

#define INP_ARG_CMD_ONLY 1
#define INP_ARG_FILE_ONLY 2
#define INP_ARG_CMD_PRIORITY 3
#define INP_ARG_FILE_PRIORITY 4
#define INP_ARG_EQUAL_PRIORITY 5

namespace inpopts{
	class InpOptsClass{
		private:
			std::vector< std::tuple<char, std::string, std::string, int, bool, int, bool> > validArgList;
			std::vector<unsigned int> optInds;
			std::vector<unsigned int> cmdNumArgs;
			std::vector< std::string > cmdOpts;
			std::vector< std::vector<std::string> > cmdArgs;
			std::vector<bool> recognizedCmdOpts;
			std::vector<bool> cmdIsLong;
			std::vector<unsigned int> fileNumArgs;
			std::vector< std::string > fileOpts;
			std::vector< std::vector<std:: string> > fileArgs;
			std::vector<bool> recognizedFileOpts;
			std::vector<bool> fileIsLong;
			int argc;
			char** argv;
			unsigned int mode;
			unsigned int numRecognizedCmdOpts; // The number of reognized options so far from the command line
			unsigned int numRecognizedFileOpts; // The number of reognized options so far from the input file
			std::string optionsFile; 
			// bool getOptionsFile;
			bool isCmdOption(const std::string, bool &);
			bool isCmdOption(const std::string);
			void gatherCmdOpts();
			void printCmdOpts();
			void gatherFileOpts();
			void printFileOpts();
			unsigned int checkCmdOpt(const char, const std::string, std::vector<std::string> &);
			unsigned int checkFileOpt(const char, const std::string, std::vector<std::string> &);
			std::string lastAdded;

			/* convert(A, B) converts the strings in the vector A into the correct type for the vector B.
			 * If the type of B is not supported, an exception is thrown */
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<std::string> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<char> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<bool> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<int> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<long> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<long long> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<float> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<double> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<long double> &argsAsCorrectType);	
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<unsigned int> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<unsigned long> &argsAsCorrectType);
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<unsigned long long> &argsAsCorrectType);			

			/* This template catches other types that cannot currently be converted and throws an exception */
			template <typename T>
			static void convert(const std::vector<std::string> &argsAsStrings, std::vector<T> &argsAsCorrectType){
					throw inpopts::CannotConvert{ };
			}

			static std::string strToUpper(const std::string);
			static void trimLWSpace(std::string &);
			static void trimRWSpace(std::string &);
			static std::string splitAtFirstWS(std::string &);
		public:
			/* Add an argument */
			template <typename T>
			void addOpt(const char opt_short, const std::string opt_long, const std::string help_text, value<T> &val){
				bool optUsed;

				lastAdded = "";
				if (opt_short != '\0'){
					lastAdded = lastAdded + opt_short;
					if (opt_long.length() > 0){
						lastAdded = lastAdded + "/";
					}
				}
				if (opt_long.length() > 0){
					lastAdded = lastAdded + opt_long;
				}

				/* First, check that T is a type that we can deal with */
				if ((typeid(T) != typeid(unsigned int)) &&(typeid(T) != typeid(int)) && (typeid(T) != typeid(float)) && (typeid(T) != typeid(double)) && (typeid(T) != typeid(bool)) && (typeid(T) != typeid(char)) && (typeid(T) != typeid(std::string))){
					throw inpopts::BadType{ };
				}

				/* Add to the list of valid arguments */
				std::tuple<char, std::string, std::string, int, bool, int, bool> tup (opt_short, opt_long, help_text, val.minArgs, val.minArgsSpec, val.maxArgs, val.maxArgsSpec);
				validArgList.push_back(tup);

				/* Check if this argument was used, either at the command line or in an input file */
				std::vector<std::string> argsAsStrings; // argsAsStrings will store the arguments to the option as a vector of strings
				optUsed = checkOpt(opt_short, opt_long, argsAsStrings, true);

				/* If the option has been used, convert the arguments to the correct type and then attempt to validate and assign them */
				if (optUsed){
					std::vector<T> argsAsCorrectType;
					convert(argsAsStrings, argsAsCorrectType);
					// std::cout << "argsAsCorrectType: \n";
					// for (typename std::vector<T>::iterator it = argsAsCorrectType.begin(); it < argsAsCorrectType.end(); ++it){
					// 	std::cout << "\t" << *it;
					// }
					// std::cout << std::endl;
					// std::cout << "validateAndAssign() returns " << val.validateAndAssign(argsAsCorrectType) << std::endl;
					try {
						val.validateAndAssign(argsAsCorrectType);
					}
					catch (const std::exception &e){
						std::string tmp_str = "Validation for the option " + lastAdded + " failed with the following error message: \n" + e.what();
						throw ValidationFailure(tmp_str);
					}
				}
				/* If the option has not been used but a default value was specified, assign that value */
				else {
					if (val.defSpec){
						val.useDefault();
					}
				}

			}

			template <typename T>
			void addOpt(const char opt_short, const std::string help_text, value<T> &val){
				const std::string opt_long = "";

				addOpt(opt_short, opt_long, help_text, val);
			}

			template <typename T>
			void addOpt(const std::string opt_long, const std::string help_text, value<T> &val){
				const char opt_short = '\0';

				addOpt(opt_short, opt_long, help_text, val);
			}
			
			// template <typename T>
			void addOpt(const char opt_short, const std::string opt_long, const std::string help_text){
				bool tmp;
				value<bool> val(tmp);

				addOpt(opt_short, opt_long, help_text, val);
			}

			bool checkOpt(const char, const std::string, std::vector<std::string> &, const bool);
			std::string listOpts();
			bool Used(const char);
			bool Used(const std::string);
			std::string Priority();
			void UnrecognizedOpts(std::string &);
			std::string UnrecognizedOpts();
			std::string LastAttemptedAdd();
			InpOptsClass(const int, char**);
			InpOptsClass(const int, char**, const std::string, std::string);
			InpOptsClass(const std::string);
	};
}

#endif
