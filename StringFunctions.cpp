#include "StringFunctions.hpp"

using namespace std;

string strToUpper(const string str){
	string str2 = str;
	unsigned int len = str.length();

	for (unsigned int it = 0; it < len; ++it){
		str2[it] = toupper(str2[it]);
	}

	return str2;
}

/* trimLWSpace(string &str) trims any whitespace at the beginning of str */
void trimLWSpace(string &str){
	locale loc;
	unsigned int str_len = str.length();
	unsigned int it;

	if (str_len == 0){
		return;
	}

	for (it = 0; it < str_len; ++it){
		if (!isspace(str[it], loc))
			break;
	}

	str = str.substr(it, str_len - it);
}

// /* trimLWSpace(string &str) trims any whitespace at the end of str */
// void trimRWSpace(string &str){
// 	locale loc;
// 	unsigned int str_len = str.length();
// 	unsigned int it;

// 	if (str_len == 0){
// 		return;
// 	}

// 	for (it = str_len; it > 0; --it){
// 		// cout << "\tstr[" << it << "] = " << str[it] << endl;
// 		if (!isspace(str[it-1], loc))
// 			break;
// 	}

// 	str = str.substr(0, it + 1);

// 	// cout << "str went from " << str_len << " to " << str.length() << " characters" << endl;
// 	// cout << "str is now = " << str << endl;
// }

/* trimRWSpace(string &str) trims any whitespace at the end of str */
void trimRWSpace(string &str){
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
string splitAtFirstWS(string &str1){
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