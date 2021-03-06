#ifndef INP_ARGS_VALUE_STRING_HPP
#define INP_ARGS_VALUE_STRING_HPP

namespace inpopts{
	template <>
	class value<std::string>{
		public:
			/* 1. Variables storing info. about possible values */
			std::string minVal; // Minimum allowed value
			std::string maxVal; // Maximum allowed value
			std::vector<std::string> valList; // Vector of allowed values
			std::string defVal; // Default value
			bool defValBool; // Boolean default value - only used when std::string is of type bool

			/* 2. Boolean variables indicating whether the variables in (1) above have been specified or not */
			bool rangeSpec; // TRUE if a range of values has been specified - maxSpec and minSpec should always be TRUE if rangeSpec is TRUE
			bool maxSpec; // TRUE if a maximum value has been specified
			bool minSpec; // TRUE if a minimum value has been specified
			bool valListSpec; // TRUE if a vector of allowed values has been specified
			bool defSpec; // TRUE if a default value has been specified

			bool vecInUse; // TRUE if we are dealing with a vector of arguments - ALWAYS check this before using varRef or vecVarRef

			/* 3. Variables storing info. about the possible number of arguments to an option */
			int minArgs; // Minimum number of arguments for an option
			int maxArgs; // Maximum number of arguments for an option
			int exactNum; // Exact number of arguments for an option

			/* 4. Boolean variables indicating whether the variables in (3) above have been specified or not */
			bool minArgsSpec; // TRUE if a minimum number of arguments has been specified
			bool maxArgsSpec; // TRUE if a maximum number of arguments has been specified
			bool rangeNumSpec; // TRUE if a range of numbers of arguments has been specified - minArgsSpec and maxArgsSpec should always be TRUE 
								//and exactNumSpec should always be FALSE if rangeNumSpec is TRUE
			bool exactNumSpec; // TRUE if an exact number of arguments has been specified - minArgsSpec and maxArgsSpec should always be TRUE 
								//and rangeNumSpec should always be FALSE if exactNumSpec is TRUE

			/* 5. Boolean variables indicating what type of variables are expected as arguments (NB the arguments may not necessarily be
			 *	  provided in this form by the user, these variables indicate how they will be stored in the program) */
			bool typeStr; // TRUE if the arguments are strings
			bool typeBool; // TRUE if the arguments are boolean
			bool typeNum; // TRUE if the arguments are numeric (types other than string/boolean types are currently assumed to be numeric)


			/* setRange(a, b) is used to set a range of possible values. It returns a reference to the current object.
			 *		If a list of values has been previously specified, it is overridden.
			 *
			 * 		rangeSpec is set to TRUE
			 * 		maxSpec is set to TRUE
			 * 		minSpec is set to TRUE
			 *		valListSpec is set to FALSE
			 */
			value& setRange(std::string a, std::string b){
				if (a < b){
					minVal = a;
					maxVal = b;
				}
				else if (b < a){
					minVal = b;
					maxVal = a; 
				}
				else {
					minVal = maxVal = a;
				}

				rangeSpec = true;
				maxSpec = true;
				minSpec = true;
				valListSpec = false;
				valList.resize(0);

				if (defSpec){
					if (defVal > maxVal){
						throw inpopts::MaxLessThanDefault { };
					}
					if (defVal < minVal){
						throw inpopts::MinGreaterThanDefault { };
					}
				}

				return *this;
			}

			// const value& setRange(std::string a, std::string b) const {
			// 	if (a < b){
			// 		minVal = a;
			// 		maxVal = b;
			// 	}
			// 	else if (b < a){
			// 		minVal = b;
			// 		maxVal = a; 
			// 	}
			// 	else {
			// 		minVal = maxVal = a;
			// 	}

			// 	rangeSpec = true;
			// 	maxSpec = true;
			// 	minSpec = true;
			// 	valListSpec = false;
			// 	valList.resize(0);

			// 	if (defSpec){
			// 		if (defVal > maxVal){
			// 			throw inpopts::MaxLessThanDefault { };
			// 		}
			// 		if (defVal < minVal){
			// 			throw inpopts::MinGreaterThanDefault { };
			// 		}
			// 	}

			// 	return *this;
			// }

			/* setMax(a) is used to set a maximum value. It returns a reference to the current object.
			 *		If a range, minimum value or list of allowed values have been previously set, they are overridden
			 *
			 *		maxSpec is set to TRUE
			 *		rangeSpec is set to FALSE
			 *		minSpec is set to FALSE
			 *		valListSpec is set to FALSE
			 */
			value& setMax(std::string inpMaxVal){
				maxVal = inpMaxVal;
				
				rangeSpec = false;
				maxSpec = true;
				minSpec = false;
				valListSpec = false;
				valList.resize(0);

				if (defSpec){
					if (defVal > maxVal){
						throw inpopts::MaxLessThanDefault { };
					}
				}							

				return *this;
			}

			// const value& setMax(std::string inpMaxVal) const {
			// 	maxVal = inpMaxVal;
				
			// 	rangeSpec = false;
			// 	maxSpec = true;
			// 	minSpec = false;
			// 	valListSpec = false;
			// 	valList.resize(0);

			// 	if (defSpec){
			// 		if (defVal > maxVal){
			// 			throw inpopts::MaxLessThanDefault { };
			// 		}
			// 	}				

			// 	return *this;
			// }			

			/* setMin(a) is used to set a maximum value. It returns a reference to the current object.
			 *		If a range, maximum value or list of allowed values have been previously set, they are overridden
			 *
			 *		minSpec is set to TRUE
			 *		rangeSpec is set to FALSE
			 *		maxSpec is set to FALSE
			 *		valListSpec is set to FALSE
			 */			
			value& setMin(std::string inpMinVal){
				minVal = inpMinVal;
				
				rangeSpec = false;
				maxSpec = false;
				minSpec = true;
				valListSpec = false;
				valList.resize(0);

				if (defSpec){
					if (defVal > minVal){
						throw inpopts::MinGreaterThanDefault { };
					}
				}				

				return *this;
			}

			// const value& setMin(std::string inpMinVal) const {
			// 	minVal = inpMinVal;
				
			// 	rangeSpec = false;
			// 	maxSpec = false;
			// 	minSpec = true;
			// 	valListSpec = false;
			// 	valList.resize(0);

			// 	if (defSpec){
			// 		if (defVal > minVal){
			// 			throw inpopts::MinGreaterThanDefault { };
			// 		}
			// 	}

			// 	return *this;
			// }			

			/* setMax(a) is used to set a maximum value. It returns a reference to the current object.
			 *		If a range, maximum or minimum value have been previously set, they are overridden
			 *
			 *		valListSpec is set to TRUE
			 *		rangeSpec is set to FALSE
			 *		maxSpec is set to FALSE
			 *		minSpec is set to FALSE
			 */			
			value& setValList(std::vector<std::string> inpValList){
				valList = inpValList;

				// std::cout << "Setting the value list using the vecor: " <<std::endl;
				// for (std::vector<std::string>::iterator it = valList.begin(); it < valList.end(); ++it){
				// 	std::cout << "|" << *it << "|" << std::endl;
				// }
				// std::cout << std::endl;

				rangeSpec = false;
				maxSpec = false;
				minSpec = false;
				valListSpec = true;

				bool notInList = true;

				if (defSpec){
					for (typename std::vector<std::string>::iterator it = valList.begin(); it < valList.end(); ++it){
						if (defVal == *it){
							notInList = false;
							break;
						}
						if (notInList){
							throw inpopts::ListDoesNotContainDefault{ };
						}
					}
				}

				return *this;
			}

			// const value& setValList(std::vector<std::string> inpValList) const {
			// 	valList = inpValList;

			// 	rangeSpec = false;
			// 	maxSpec = false;
			// 	minSpec = false;
			// 	valListSpec = true;

			// 	bool notInList = true;

			// 	if (defSpec){
			// 		for (typename std::vector<std::string>::iterator it = valList.begin(); it < valList.end(); ++it){
			// 			if (defVal == *it){
			// 				notInList = false;
			// 				break;
			// 			}
			// 			if (notInList){
			// 				throw inpopts::ListDoesNotContainDefault{ };
			// 			}
			// 		}
			// 	}

			// 	return *this;
			// }			

			/* defaultValue(a) is used to set a default value. It returns a reference to the current object.
			 */
			value& defaultValue(std::string inpDefVal){
				defVal = inpDefVal;

				defSpec = true;

				bool notInList = true;

				/* If acceptable value have previously been specified, we check that the default value
				 * is acceptable and throw an exception if it is not
				 */
				/* Check whether the default value is contained in the list of allowed values */
				if (valListSpec){
					for (typename std::vector<std::string>::iterator it = valList.begin(); it < valList.end(); ++it){
						if (defVal == *it){
							notInList = false;
							break;
						}
						if (notInList){
							throw inpopts::DefaultOutsideRange{ };
						}
					}
				}
				else {
					/* Check whether the default value exceeds the maximum allowed value */
					if (maxSpec){
						if (defVal > maxVal){
							throw inpopts::DefaultOutsideRange { };
						}
					}
					/* Check whether the default value is less than the minimum allowed value */
					if (minSpec){
						if (defVal < minVal){
							throw inpopts::DefaultOutsideRange { };
						}
					}
				}

				return *this;
			}

			// const value& defaultValue(std::string inpDefVal) const {
			// 	defVal = inpDefVal;

			// 	defSpec = true;

			// 	bool notInList = true;

			// 	/* If acceptable value have previously been specified, we check that the default value
			// 	 * is acceptable and throw an exception if it is not
			// 	 */
			// 	/* Check whether the default value is contained in the list of allowed values */
			// 	if (valListSpec){
			// 		for (typename std::vector<std::string>::iterator it = valList.begin(); it < valList.end(); ++it){
			// 			if (defVal == *it){
			// 				notInList = false;
			// 				break;
			// 			}
			// 			if (notInList){
			// 				throw inpopts::DefaultOutsideRange{ };
			// 			}
			// 		}
			// 	}
			// 	else {
			// 		/* Check whether the default value exceeds the maximum allowed value */
			// 		if (maxSpec){
			// 			if (defVal > maxVal){
			// 				throw inpopts::DefaultOutsideRange { };
			// 			}
			// 		}
			// 		/* Check whether the default value is less than the minimum allowed value */
			// 		if (minSpec){
			// 			if (defVal < minVal){
			// 				throw inpopts::DefaultOutsideRange { };
			// 			}
			// 		}
			// 	}

			// 	return *this;		
			// }		

			value& numArgs(int n_exact){
				exactNum = n_exact;
				minArgs = n_exact;
				maxArgs = n_exact;

				if (n_exact >= 0){
					exactNumSpec = true;
					rangeNumSpec = false;
					minArgsSpec = true;
					maxArgsSpec = true;
				}

				return *this;
			}

			// const value& numArgs(unsigned int n_exact) const {
			// 	exactNum = n_exact;
			// 	minArgs = n_exact;
			// 	maxArgs = n_exact;

			// 	if (n_exact >= 0){
			// 		exactNumSpec = true;
			// 		rangeNumSpec = false;
			// 		minArgsSpec = true;
			// 		maxArgsSpec = true;
			// 	}

			// 	return *this;
			// }			

			value& numArgs(int n_min, int n_max){
				minArgs = n_min;
				maxArgs = n_max;
				
				if (minArgs >= 0){
					minArgsSpec = true;
				}
				if (maxArgs >= 0){
					maxArgsSpec = true;
				}

				/* If a minimum and maximum number of arguments have both been specified, validate 
				 * them and check if we need to set the exactNumSpec and rangeNumSpec flags
				 */
				if (minArgsSpec && maxArgsSpec){
					/* Check that the maximum number of arguments is greater than or equal to the 
					 * minium - if not, throw an exception
					 */
					if (n_max < n_min){
						throw inpopts::MaxArgsLessThanMinArgs{ };
					}
					if (n_min == n_max){
						exactNumSpec = true;
						rangeNumSpec = false;
					}
					else {
						exactNumSpec = false;
						rangeNumSpec = true;
					}
				}

				return *this;
			}

			// const value& numArgs(unsigned int n_min, unsigned int n_max) const {
			// 	minArgs = n_min;
			// 	maxArgs = n_max;
				
			// 	if (minArgs >= 0){
			// 		minArgsSpec = true;
			// 	}
			// 	if (maxArgs >= 0){
			// 		maxArgsSpec = true;
			// 	}

			// 	/* If a minimum and maximum number of arguments have both been specified, validate 
			// 	 * them and check if we need to set the exactNumSpec and rangeNumSpec flags
			// 	 */
			// 	if (minArgsSpec && maxArgsSpec){
			// 		/* Check that the maximum number of arguments is greater than or equal to the 
			// 		 * minium - if not, throw an exception
			// 		 */
			// 		if (maxArgs < minArgs){
			// 			throw inpopts::MaxArgsLessThanMinArgs{ };
			// 		}
			// 		if (minArgs == maxArgs){
			// 			exactNumSpec = true;
			// 			rangeNumSpec = false;
			// 		}
			// 		else {
			// 			exactNumSpec = false;
			// 			rangeNumSpec = true;
			// 		}
			// 	}

			// 	return *this;
			// }	

			value& minNumArgs(int n_min){
				minArgs = n_min;

				if (minArgs >= 0){
					exactNumSpec = false;
					rangeNumSpec = false;
					minArgsSpec = true;
					maxArgsSpec = false;
				}

				/* If a minimum and maximum number of arguments have both been specified, validate 
				 * them and check if we need to set the exactNumSpec and rangeNumSpec flags
				 */
				if (minArgsSpec && maxArgsSpec){
					/* Check that the maximum number of arguments is greater than or equal to the 
					 * minium - if not, throw an exception
					 */
					if (maxArgs < n_min){
						throw inpopts::MaxArgsLessThanMinArgs{ };
					}
					if (n_min == maxArgs){
						exactNumSpec = true;
						rangeNumSpec = false;
					}
					else {
						exactNumSpec = false;
						rangeNumSpec = true;
					}
				}				

				return *this;
			}			

			// const value& minNumArgs(unsigned int n_min) const {
			// 	minArgs = n_min;

			// 	if (minArgs >= 0){
			// 		exactNumSpec = false;
			// 		rangeNumSpec = false;
			// 		minArgsSpec = true;
			// 		maxArgsSpec = false;
			// 	}

			// 	/* If a minimum and maximum number of arguments have both been specified, validate 
			// 	 * them and check if we need to set the exactNumSpec and rangeNumSpec flags
			// 	 */
			// 	if (minArgsSpec && maxArgsSpec){
			// 		/* Check that the maximum number of arguments is greater than or equal to the 
			// 		 * minium - if not, throw an exception
			// 		 */
			// 		if (maxArgs < n_min){
			// 			throw inpopts::MaxArgsLessThanMinArgs{ };
			// 		}
			// 		if (n_min == maxArgs){
			// 			exactNumSpec = true;
			// 			rangeNumSpec = false;
			// 		}
			// 		else {
			// 			exactNumSpec = false;
			// 			rangeNumSpec = true;
			// 		}
			// 	}				

			// 	return *this;
			// }

			value& maxNumArgs(int n_max){
				maxArgs = n_max;

				if (maxArgs >= 0){
					exactNumSpec = false;
					rangeNumSpec = false;
					minArgsSpec = false;
					maxArgsSpec = true;
				}

				/* If a minimum and maximum number of arguments have both been specified, validate 
				 * them and check if we need to set the exactNumSpec and rangeNumSpec flags
				 */
				if (minArgsSpec && maxArgsSpec){
					/* Check that the maximum number of arguments is greater than or equal to the 
					 * minium - if not, throw an exception
					 */
					if (maxArgs < minArgs){
						throw inpopts::MaxArgsLessThanMinArgs{ };
					}
					if (minArgs == maxArgs){
						exactNumSpec = true;
						rangeNumSpec = false;
					}
					else {
						exactNumSpec = false;
						rangeNumSpec = true;
					}
				}

				return *this;
			}

			// const value& maxNumArgs(unsigned int n_max) const {
			// 	maxArgs = n_max;

			// 	if (maxArgs >= 0){
			// 		exactNumSpec = false;
			// 		rangeNumSpec = false;
			// 		minArgsSpec = false;
			// 		maxArgsSpec = true;
			// 	}

			// 	/* If a minimum and maximum number of arguments have both been specified, validate 
			// 	 * them and check if we need to set the exactNumSpec and rangeNumSpec flags
			// 	 */
			// 	if (minArgsSpec && maxArgsSpec){
			// 		/* Check that the maximum number of arguments is greater than or equal to the 
			// 		 * minium - if not, throw an exception
			// 		 */
			// 		if (maxArgs < minArgs){
			// 			throw inpopts::MaxArgsLessThanMinArgs{ };
			// 		}
			// 		if (minArgs == maxArgs){
			// 			exactNumSpec = true;
			// 			rangeNumSpec = false;
			// 		}
			// 		else {
			// 			exactNumSpec = false;
			// 			rangeNumSpec = true;
			// 		}
			// 	}

			// 	return *this;
			// }

			bool validateAndAssign(std::vector<std::string> &inp){
				bool valid = false;

				// std::cout << "validateAndAssin()" << std::endl;

				// bool inpBool;
				// std::vector<bool> inpBoolVec;

				int nArgsIn = inp.size();

				/* If the vector inp is empty but the type of std::string is bool, then this is a boolean switch. If there
				 * is a default value, we set the value to be the opposite, otherwise we set it to be true 
				 */
				// if (typeBool){
				// 	if (nArgsIn == 0){
				// 		if (defSpec){
				// 			varRef = defValBool ? false : true;
				// 			// varRef = !defVal;
				// 		}
				// 		else {
				// 			varRef = true;
				// 		}
				// 		return true;
				// 	}
				// }

				/* Check that an acceptable number of arguments has been provided */
				if (!validateNumArgs(nArgsIn)){
					// std::cout << "failed validateNumArgs(" << nArgsIn << ")" << std::endl;
					return false;
				}

				/* At this point we know we have an acceptable number of arguments */

				/* Check that the values of the arguments that have been provided are valid */

				/* If we are dealing with strings, we do things a little differently... */
				if (typeStr){
					valid = validateString(inp);
					// std::cout << "validateString(inp) returned " << valid << std::endl;
				}
				// /* Likewise we deal with boolean variables a little differently... */
				// else if (typeBool){
				// 	valid = true;
				// }
				//  Otherwise we can assume we are dealing with a numeric type 
				// else if (typeNum){
				// 	valid = validateNumeric(inp);
				// }

				/* If the input was valid (i.e., valid == TRUE), then we can assign the variable */
				if (valid){
					// std::cout << "vecInUse = " << vecInUse << std::endl;
					// std::cout << "nArgsIn = " << nArgsIn << std::endl;
					// std::cout << "inp = " << std::endl;
					// for (std::vector<std::string>::iterator it = inp.begin(); it < inp.end(); ++it){
					// 	std::cout << "\t|" << *it << "|";
					// }
					std::cout << std::endl;
					if (vecInUse){
						vecVarRef = inp;
					}
					else if (nArgsIn == 1) {
						varRef = *(inp.begin());
					}
					else {
						throw inpopts::NeedToUseVector{ };
					}

					// std::cout << "varRef = " << varRef << std::endl;

					return true;
				}
				else {
					return false;
				}
			}

			void useDefault(){
				varRef = defVal;
			}

			/* Constructor for the value class */
			value(std::string &inpVarRef) : varRef(inpVarRef), vecVarRef(emptyVector) {				
				vecInUse = false;

				rangeSpec = false;
				maxSpec = false;
				minSpec = false;
				valListSpec = false;
				valList.resize(0);
				defSpec = false;

				rangeNumSpec = false;
				maxArgsSpec = false;
				minArgsSpec = false;
				exactNumSpec = false;

				typeStr = (typeid(std::string) == typeid(std::string)); // TRUE if we are dealing with arguments of type string
				typeBool = (typeid(std::string) == typeid(bool)); // TRUE if we are dealing with arguments of type bool
				typeNum = (!(typeStr || typeBool)); // TRUE if we are dealing with arguments of a numeric type				

				defSpec = false;
			}

			/* Constructor for the value class where a vector of inputs is expected */
			value(typename std::vector<std::string> &inpVecVarRef) : varRef(emptyT), vecVarRef(inpVecVarRef) {
				vecInUse = true;

				rangeSpec = false;
				maxSpec = false;
				minSpec = false;
				valListSpec = false;
				valList.resize(0);
				defSpec = false;

				rangeNumSpec = false;
				maxArgsSpec = false;
				minArgsSpec = false;
				exactNumSpec = false;

				typeStr = (typeid(std::string) == typeid(std::string)); // TRUE if we are dealing with arguments of type string
				typeBool = (typeid(std::string) == typeid(bool)); // TRUE if we are dealing with arguments of type bool
				typeNum = (!(typeStr || typeBool)); // TRUE if we are dealing with arguments of a numeric type	

				defSpec = false;
			}

		private:
			std::string& varRef;
			std::vector<std::string>& vecVarRef;
			std::vector<std::string> emptyVector;
			std::string emptyT;

			bool validateNumArgs(int nArgsIn){
				if (exactNumSpec){
					if (nArgsIn != exactNum){
						return false;
					}
				}

				if (minArgsSpec){
					if (nArgsIn < minArgsSpec){
						return false;
					}
				}

				if (maxArgsSpec){
					if (nArgsIn > maxArgsSpec){
						return false;
					}
				}

				return true;
			}

			/* Functions for validating arguments - these only check the values of the arguments and assume that any other
			 * inportant checks (such as checking that an acceptable number of arguments was provided) are done elsewhere
			 */
			bool validateString(std::vector<std::string> &inp){
				bool ret;
				// std::cout << "rangeSpec = " << rangeSpec << std::endl;
				// std::cout << "minSpec = " << minSpec << std::endl;
				// std::cout << "maxSpec = " << maxSpec << std::endl;
				// std::cout << "valListSpec = " << valListSpec << std::endl;
				// try {
					if (rangeSpec){
						ret = validateRange(inp);
						if (!ret){
							throw OutsideRange{ };
						}
					}
					else if (minSpec){
						ret = validateMin(inp);
						if (!ret){
							throw LTMin{ };
						}
					}
					else if (maxSpec){
						ret = validateMax(inp);
						if (!ret){
							throw GTMax{ };
						}
					}
					else if (valListSpec){
						ret = validateStringList(inp);
						if (!ret){
							throw NotInList{ };
						}
					}
					else {
						/* If we get here, no restrictions have been specified, so there
						 * is no test to pass, i.e., the input is always considered valid
						 */
						ret = true;
					}
				// }
				// catch

				return ret;
			}

			bool validateRange(std::string &inp){
				if (inp < minVal || inp > maxVal){
					return false;
				}
				else {
					return true;
				}
			}

			bool validateRange(std::vector<std::string> &inp){
				for (typename std::vector<std::string>::iterator it = inp.begin(); it < inp.end(); ++it){
					if (*it < minVal || *it > maxVal){
						return false;
					}
				}

				return true;
			}

			bool validateMax(std::string &inp){
				if (inp > maxVal){
					return false;
				}
				else {
					return true;
				}
			}

			bool validateMax(std::vector<std::string> &inp){
				for (typename std::vector<std::string>::iterator it = inp.begin(); it < inp.end(); ++it){
					if (*it > maxVal){
						return false;
					}
				}

				return true;
			}

			bool validateMin(std::string &inp){
				if (inp > minVal){
					return false;
				}
				else {
					return true;
				}
			}

			bool validateMin(std::vector<std::string> &inp){
				for (typename std::vector<std::string>::iterator it = inp.begin(); it < inp.end(); ++it){
					if (*it > minVal){
						return false;
					}
				}

				return true;
			}

			bool validateStringList(std::string &inp){
				bool valid; // TRUE if the element of inp that has most recently been checked was valid
				
				valid = false;

				// std::cout << "vecVarRef.size() = " << vecVarRef.size() << std::endl;
				// std::cout << "vecInUse = " << vecInUse << std::endl;

				for (typename std::vector<std::string>::iterator it2 = valList.begin(); it2 < vecVarRef.end(); ++it2){
							// std::cout << "Comparing |" << inp << "| with |" << *it2 << "|" << std::endl;
					/* If inp == *it2, inp is a valid input */
					if (!inp.compare(*it2)){
								// std::cout << "\tFAIL!!" << std::endl;
						valid = true;
						break;
					}
				}
				
				/* If valid is FALSE, there has been an invalid input detected - return FALSE */
				if (!valid){
					return false;
				}

				/* If we reach this point, the input is valid */
				return true;				
			}

			bool validateStringList(std::vector<std::string> &inp){
				bool valid = true; // TRUE if the element of inp that has most recently been checked was valid
				bool all_valid = true; // TRUE if all of the elements of inp that have been checked so far have been valid

				// std::cout << "vecVarRef.size() = " << vecVarRef.size() << std::endl;
				// std::cout << "valListSpec = " << valListSpec << std::endl;

				// if (typeid(std::string) == typeid(std::string)){
					for (typename std::vector<std::string>::iterator it1 = inp.begin(); it1 < inp.end(); ++it1){
						valid = false;
						for (typename std::vector<std::string>::iterator it2 = valList.begin(); it2 < valList.end(); ++it2){
						// for (typename std::vector<std::string>::iterator it2 = vecVarRef.begin(); it2 < vecVarRef.end(); ++it2){

							/* If *it1 == *it2, the current element of inp is a valid input, we can move onto the next element of inp */
							if (!(*it1).compare(*it2)){
								valid = true;
								break;
							}
						}

						/* Update all_valid */
						all_valid = all_valid && valid;

						/* If all_valid is no longer true, there has been an invalid input detected - return FALSE */
						if (!all_valid){
							return false;
						}
					}

					/* If we reach this point, all inputs have been valid */
					return true;					
				// }
			}

			static std::string strToUpper(std::string str){
				std::string str2 = str;
				unsigned int len = str.length();

				for (unsigned int it = 0; it < len; ++it){
					str2[it] = toupper(str2[it]);
				}

				return str2;
			}		
	};
}

#endif