#ifndef EDGE_LIST_CLASS_EXCEPTIONS
#define EDGE_LIST_CLASS_EXCEPTIONS

#include <exception>
#include <string>

class BadVNDeg{
public:
	BadVNDeg(const int &deg_in, const int &min_deg_in, const int &max_deg_in) : min_deg(min_deg_in), max_deg(max_deg_in), deg(deg_in) { };
	virtual const char* what() const throw(){
		std::string msg = "VN degrees must lie between " + std::to_string(min_deg) + " and " + std::to_string(max_deg) + " (" + std::to_string(deg) + " was provided)\n";
		return msg.c_str();
	}	
private:
	const int min_deg;
	const int max_deg;
	const int deg;
};

class BadCNDeg{
public:
	BadCNDeg(const int &deg_in, const int &min_deg_in, const int &max_deg_in) : min_deg(min_deg_in), max_deg(max_deg_in), deg(deg_in) { };
	virtual const char* what() const throw(){
		std::string msg = "CN degrees must lie between " + std::to_string(min_deg) + " and " + std::to_string(max_deg) + " (" + std::to_string(deg) + " was provided)\n";
		return msg.c_str();
	}	
private:
	const int min_deg;
	const int max_deg;
	const int deg;
};

class UnusedVN{
public:
	UnusedVN(const int &VN_in, const int &max_VN_in) : VN(VN_in) , max_VN(max_VN_in) { };
	virtual const char* what() const throw(){
		std::string msg = "VN #" + std::to_string(VN) + " was not specified in the input file, but VNs with higher numbers (up to VN #" + std::to_string(max_VN) + ") were!\n";
		return msg.c_str();
	}
private:
	const int VN;
	const int max_VN;
};

class UnusedCN{
public:
	UnusedCN(const int &CN_in, const int &max_CN_in) : CN(CN_in) , max_CN(max_CN_in) { };
	virtual const char* what() const throw(){
		std::string msg = "CN #" + std::to_string(CN) + " was not specified in the input file, but CNs with higher numbers (up to CN #" + std::to_string(max_CN) + ") were!\n";
		return msg.c_str();
	}
private:
	const int CN;
	const int max_CN;
};

struct 	CantReadFileVersion{ };

#endif