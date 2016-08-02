#ifndef EDGE_LIST_CLASS_HPP
#define EDGE_LIST_CLASS_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "StringFunctions.hpp"
#include "MatrixFileExceptions.hpp"
#include "EdgeListClassExceptions.hpp"

#define EDGE_LIST_BIN_FILE_VERSION 0 // A version number for the file format used by the binary file functions - should help to avoid incorrectly interpreting files after changes are made to these functions

/* EdgeList class - a class for bipartite graphs for LDPC codes */
class EdgeList{
public:
	int *CNConnections; // Array containing the list of VNs to which each CN is connected - the range of indices pertaining to CN #x is given by CNConnInds(x)
	int *VNConnections; // Array containing the list of CNs to which each VN is connected - the range of indices pertaining to VN #x is given by VNConnInds(x)
	int *CNDegrees; // Array containing the list of degrees for each CN
	int *VNDegrees; // Array containing the list of degrees for each VN
	int edges(); // Return the total number of edges in the graph
	int edges() const; // Return the total number of edges in the graph
	int VNs(); // Return the total number of VNs in the graph
	int VNs() const; // Return the total number of VNs in the graph
	int CNs(); // Return the total number of CNs in the graph
	int CNs() const; // Return the total number of CNs in the graph
	int VNs(const int &deg); // Return the total number of degree-deg VNs in the graph
	int VNs(const int &deg) const; // Return the total number of degree-deg VNs in the graph
	int CNs(const int &deg); // Return the total number of degree-deg CNs in the graph
	int CNs(const int &deg) const; // Return the total number of degree-deg CNs in the graph
	float fracVNs(const int &deg); // Return the fraction of degree-deg VNs in the graph
	float fracVNs(const int &deg) const; // Return the fraction of degree-deg VNs in the graph
	float fracCNs(const int &deg); // Return the fraction of degree-deg CNs in the graph
	float fracCNs(const int &deg) const; // Return the fraction of degree-deg CNs in the graph
	int maxVNDegree(); // Return the maximum VN degree
	int maxVNDegree() const; // Return the maximum VN degree
	int minVNDegree(); // Return the minimum VN degree
	int minVNDegree() const; // Return the minimum VN degree
	int maxCNDegree(); // Return the maximum CN degree
	int maxCNDegree() const; // Return the maximum CN degree
	int minCNDegree(); // Return the minimum CN degree
	int minCNDegree() const; // Return the minimum CN degree
	int VNDegree(const int &VN); // Return the degree of VN #VN
	int VNDegree(const int &VN) const; // Return the degree of VN #VN
	int CNDegree(const int &CN); // Return the degree of CN #CN
	int CNDegree(const int &CN) const; // Return the degree of CN #CN
	void VNConnInds(const int &VN, int &start_ind, int &end_ind); // Gives the first and last indices in VNConnections pertaining to VN #VN
	void VNConnInds(const int &VN, int &start_ind, int &end_ind) const; // Gives the first and last indices in VNConnections pertaining to VN #VN
	void CNConnInds(const int &CN, int &start_ind, int &end_ind); // Gives the first and last indices in CNConnections pertaining to CN #CN
	void CNConnInds(const int &CN, int &start_ind, int &end_ind) const; // Gives the first and last indices in CNConnections pertaining to CN #CN
	std::string listAllVNConnections(); // Returns a string listing for each VN all of the connected CNs
	std::string listAllVNConnections() const; // Returns a string listing for each VN all of the connected CNs
	std::string listAllCNConnections(); // Returns a string listing for each CN all of the connected VNs
	std::string listAllCNConnections() const; // Returns a string listing for each CN all of the connected VNs
	std::string listVNDegreeAmts(); // Returns a string listing the number and fraction of VNs of each degree
	std::string listVNDegreeAmts() const; // Returns a string listing the number and fraction of VNs of each degree
	std::string listCNDegreeAmts(); // Returns a string listing the number and fraction of CNs of each degree
	std::string listCNDegreeAmts() const; // Returns a string listing the number and fraction of CNs of each degree
	bool load(const std::string &file_name, const std::string &file_type); // Load an edge list from a file
	bool load(const std::string &file_name, const std::string &file_type) const; // Load an edge list from a file
	bool save(const std::string &file_name, const std::string &file_type); // Save an edge list to a file
	bool save(const std::string &file_name, const std::string &file_type) const; // Save an edge list to a file

	/* Constructors and Destructors */
	EdgeList(const std::string &file_name, const std::string &file_type); // Constructor which loads an edge list from a file
	EdgeList(); // Default constructor
	~EdgeList(); // Destructor
private:
	int *CNConnectionInds; // Array containing the first and last indices of CNConnections pertaining to each CN
	int *VNConnectionInds; // Array containing the first and last indices of VNConnections pertaining to each VN
	int *numVNsEachDegree; // Array containing the numbers of VNs of each degree from degree-0 to the maximum degree
	int *numCNsEachDegree; // Array containing the numbers of CNs of each degree from degree-0 to the maximum degree
	int numEdges; // Total number of edges in the graph
	int numVNs; // Total number of VNs in the graph
	int numCNs; // Total number of CNs in the graph
	int maxVNDeg; // Maximum VN degree
	int minVNDeg; // Minimum VN degree
	int maxCNDeg; // Maximum CN degree
	int minCNDeg; // Minimum CN degree
	bool inUse; // TRUE when the current object already contains a graph, FALSE otherwise
	bool reset(); // Clears all variables and returns the current object to the state given by the default constructor
	void loadTxt(const std::string &file_name); // Load an edge list from a text file
	void loadBin(const std::string &file_name); // Load an edge list from a binary file
	void saveTxt(const std::string &file_name); // Save an edge list to a text file
	void saveTxt(const std::string &file_name) const; // Save an edge list to a text file
	void saveBin(const std::string &file_name); // Save an edge list to a binary file
	void saveBin(const std::string &file_name) const; // Save an edge list to a binary file
	static unsigned int skipCommentLines(std::ifstream &ifs, const char ch); // Move the input stream past any lines beginning with the character ch
	static unsigned int skipBlankCommentLines(std::ifstream &ifs, const char ch); // Move the input stream past any lines which either are blank or begin with the character ch
	static std::string trimComments(const std::string &str, const char ch); // Remove comments (everything including and after the first occurrence of the character ch) from a string
	static std::istream& getNextNBNCLine(std::ifstream &ifs, std::string &str, const char ch); // Get the next line in the input stream which is neither blank nor a comment
	static std::istream& getNextNBNCTLine(std::ifstream &ifs, std::string &str, const char ch); // Get the next line in the input stream which is neither blank nor a comment and trim any comments from the end of the string
};

#endif