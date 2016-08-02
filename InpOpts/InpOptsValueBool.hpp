#ifndef INP_ARGS_VALUE_BOOL_HPP
#define INP_ARGS_VALUE_BOOL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

#include "InpOptsExceptions.hpp"

namespace inpopts{
	template <>
	class value<bool>{
		public:
			/* 1. Variables storing info. about possible values */
			bool defVal; // Default value

			/* 2. Boolean variables indicating whether the variables in (1) above have been specified or not */
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

			/* defaultValue(a) is used to set a default value. It returns a reference to the current object.
			 */
			value& defaultValue(bool inpDefVal){
				defVal = inpDefVal;

				defSpec = true;

				return *this;
			}	

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

			bool validateAndAssign(std::vector<bool> &inp){
				bool valid = false;

				// bool inpBool;
				// std::vector<bool> inpBoolVec;

				int nArgsIn = inp.size();

				/* If the vector inp is empty but the type of bool is bool, then this is a boolean switch. If there
				 * is a default value, we set the value to be the opposite, otherwise we set it to be true 
				 */
				// if (typeBool){
					if (nArgsIn == 0){
						if (defSpec){
							varRef = defVal ? false : true;
						}
						else {
							varRef = true;
						}
						return true;
					}
				// }

				/* Check that an acceptable number of arguments has been provided */
				if (!validateNumArgs(nArgsIn)){
					return false;
				}

				/* At this point we know we have an acceptable number of arguments */

				/* As they are of type bool, we know they must be valid boolean values */

				/* If the input was valid (i.e., valid == TRUE), then we can assign the variable */
				if (valid){
					if (vecInUse){
						vecVarRef = inp;
					}
					else if (nArgsIn == 1) {
						varRef = *(inp.begin());
					}
					else {
						throw inpopts::NeedToUseVector{ };
					}

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
			value(bool &inpVarRef) : varRef(inpVarRef), vecVarRef(emptyVector) {				
				vecInUse = false;

				defSpec = false;

				rangeNumSpec = false;
				maxArgsSpec = false;
				minArgsSpec = false;
				exactNumSpec = false;				

				defSpec = false;
			}

			/* Constructor for the value class where a vector of inputs is expected */
			value(typename std::vector<bool> &inpVecVarRef) : varRef(emptyT), vecVarRef(inpVecVarRef) {
				vecInUse = true;

				defSpec = false;

				rangeNumSpec = false;
				maxArgsSpec = false;
				minArgsSpec = false;
				exactNumSpec = false;	

				defSpec = false;
			}

		private:
			bool& varRef;
			std::vector<bool>& vecVarRef;
			std::vector<bool> emptyVector;
			bool emptyT;

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
	};
}

#endif