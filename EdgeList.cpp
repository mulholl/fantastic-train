#include "EdgeList.hpp"

/* Return the total number of edges */
int EdgeList::edges(){
	return numEdges;
}

/* Return the total number of VNs */
int EdgeList::VNs(){
	return numVNs;
}

/* Return the total number of CNs */
int EdgeList::CNs(){
	return numCNs;
}

/* Return the number of VNs of a given degree */
int EdgeList::VNs(const int &deg){
	if ((deg <= maxVNDeg) && (deg >= minVNDeg)){
		return numVNsEachDegree[deg];
	}
	else {
		throw BadVNDeg(deg, minVNDeg, maxVNDeg);
	}
}

/* Return the number of CNs of a given degree */
int EdgeList::CNs(const int &deg){
	if ((deg <= maxCNDeg) && (deg >= minCNDeg))
		return numCNsEachDegree[deg];
	else
		throw BadCNDeg(deg, minCNDeg, maxCNDeg);
}

/* Return the fraction of VNs of a given degree */
float EdgeList::fracVNs(const int &deg){
	if ((deg <= maxVNDeg) && (deg >= minVNDeg))
		return (float)numVNsEachDegree[deg] / (float) numVNs;
	else
		throw BadVNDeg(deg, minVNDeg, maxVNDeg);
}

/* Return the fraction of CNs of a given degree */
float EdgeList::fracCNs(const int &deg){
	if ((deg <= maxCNDeg) && (deg >= minCNDeg))
		return (float)numCNsEachDegree[deg] / (float) numCNs;
	else
		throw BadCNDeg(deg, minCNDeg, maxCNDeg);
}

/* Return the maximum VN degree */
int EdgeList::maxVNDegree(){
	return maxVNDeg;
}

/* Return the minimum VN degree */
int EdgeList::minVNDegree(){
	return minVNDeg;
}

/* Return the maximum CN degree */
int EdgeList::maxCNDegree(){
	return maxCNDeg;
}

/* Return the minimum CN degree */
int EdgeList::minCNDegree(){
	return minCNDeg;
}

/* Return the degree of a given VN */
int EdgeList::VNDegree(const int &VN){
	return VNDegrees[VN];
}

/* Return the degree of a given CN */
int EdgeList::CNDegree(const int &CN){
	return CNDegrees[CN];
}

/* Give the first and last indices of VNConnections that pertain to a given VN */
void EdgeList::VNConnInds(const int &VN, int &start_ind, int &end_ind){
	int tmp = 2 * VN;
	start_ind = VNConnectionInds[tmp];
	end_ind = VNConnectionInds[tmp + 1];
}

/* Give the first and last indices of CNConnections that pertain to a given CN */
void EdgeList::CNConnInds(const int &CN, int &start_ind, int &end_ind){
	int tmp = 2 * CN;
	start_ind = CNConnectionInds[tmp];
	end_ind = CNConnectionInds[tmp + 1];
}

/* Return a string listing for each VN all of the CNs to which it is connected */
std::string EdgeList::listAllVNConnections(){
	std::string ret = "";
	std::ostringstream oss;
	std::string tmp_str;
	int max_digits = std::to_string(numVNs).size();
	int lwidth = max_digits+6;
	int width = max_digits+1;

	int startInd, endInd;
	int count = 0;

	// oss << "List of all CNs connected to each VN: \n\n";

	for (int i = 0; i < numVNs; ++i){
		VNConnInds(i, startInd, endInd);


		tmp_str = "VN #" + std::to_string(i) + ": ";

		oss << std::setw(lwidth) << tmp_str;

		count = 0;

		for (int j = startInd; j <= endInd; ++j){
			int tmp = VNConnections[j];
			oss << std::setw(width) << tmp;
			count++;
			if ((count % 10) == 0 || j == endInd){
				oss << "\n" << std::setw(lwidth) << "";
			}
		}
		oss << '\n';
	}

	return oss.str();
}

/* Return a string listing for each CN all of the VNs to which it is connected */
std::string EdgeList::listAllCNConnections(){
	std::string ret = "";
	std::ostringstream oss;
	std::string tmp_str;
	int max_digits = std::to_string(numCNs).size();
	int lwidth = max_digits+6;
	int width = max_digits+1;

	int startInd, endInd;
	int count = 0;

	// oss << "List of all VNs connected to each CN: \n\n";

	for (int i = 0; i < numCNs; ++i){
		CNConnInds(i, startInd, endInd);

		tmp_str = "CN #" + std::to_string(i) + ": ";

		oss << std::setw(lwidth) << tmp_str;

		count = 0;

		for (int j = startInd; j <= endInd; ++j){
			int tmp = CNConnections[j];
			oss << std::setw(width) << tmp;
			count++;
			if ((count % 10) == 0 || j == endInd){
				oss << "\n" << std::setw(lwidth) << "";
			}
		}
		oss << '\n';
	}

	return oss.str();
}

std::string EdgeList::listVNDegreeAmts(){
	std::string ret = "";

	int degree;
	int amt;
	float frac;

	ret = ret + "Number of VNs of each degree:\n";
	ret = ret + "Degree\tNumber\tFraction\n";
	ret = ret + "========================\n";

	for (degree = minVNDeg; degree <= maxVNDeg; ++degree){
		amt = VNs(degree);

		if (amt == 0)
			continue;

		frac = fracVNs(degree);

		ret = ret + std::to_string(degree) + "\t" + std::to_string(amt) + "\t" + std::to_string(frac) + "\n";
	}

	return ret;
}

std::string EdgeList::listCNDegreeAmts(){
	std::string ret = "";

	int degree;
	int amt;
	float frac;

	ret = ret + "Number of CNs of each degree:\n";
	ret = ret + "Degree\tNumber\tFraction\n";
	ret = ret + "========================\n";

	for (int degree = minCNDeg; degree <= maxCNDeg; ++degree){
		amt = CNs(degree);

		if (amt == 0)
			continue;

		frac = fracCNs(degree);

		ret = ret + std::to_string(degree) + "\t" + std::to_string(amt) + "\t" + std::to_string(frac) + "\n";
	}

	return ret;
}



/* Load an edge list from an input file */
bool EdgeList::load(const std::string &file_name, const std::string &file_type){
	if (strToUpper(file_type).compare("TXT") == 0){
		loadTxt(file_name);
		return true;
	}
	else if (strToUpper(file_type).compare("BIN") == 0){
		// loadBin(file_name);
		return true;
	}
	else {
		return false;
	}
}

// /* Save an edge list to an output file */
// bool EdgeList::save(const std::string &file_name, const std::string &file_type){
// 	if (strToUpper(file_type).compare("TXT") == 0){
// 		saveTxt(file_name);
// 		return true;
// 	}
// 	else if (strToUpper(file_type).compare("BIN") == 0){
// 		saveBin(file_name);
// 		return true;
// 	}
// 	else {
// 		return false;
// 	}
// }

/* Constructor which loads an edge list from an input file */
EdgeList::EdgeList(const std::string &file_name, const std::string &file_type){
	numEdges = 0;
	numVNs = 0;
	numCNs = 0;
	maxVNDeg = 0;
	minVNDeg = 0;
	maxCNDeg = 0;
	minCNDeg = 0;

	inUse = false;
	
	load(file_name, file_type);
}

/* Default constructor */
EdgeList::EdgeList(){
	numEdges = 0;
	numVNs = 0;
	numCNs = 0;
	maxVNDeg = 0;
	minVNDeg = 0;
	maxCNDeg = 0;
	minCNDeg = 0;

	inUse = false;
}

/* Destructor - deallocate memory */
EdgeList::~EdgeList(){
	delete [] CNConnections;
	delete [] VNConnections;	
	delete [] CNDegrees;
	delete [] VNDegrees;
	delete [] CNConnectionInds;
	delete [] VNConnectionInds;
	delete [] numVNsEachDegree;
	delete [] numCNsEachDegree;
}

/* Clear all variables to return the object to its default state */
bool EdgeList::reset(){
	if (inUse){
		numEdges = 0;
		numVNs = 0;
		numCNs = 0;
		maxVNDeg = 0;
		minVNDeg = 0;
		maxCNDeg = 0;
		minCNDeg = 0;

		delete [] CNConnections;
		delete [] VNConnections;	
		delete [] CNDegrees;
		delete [] VNDegrees;
		delete [] CNConnectionInds;
		delete [] VNConnectionInds;
		delete [] numVNsEachDegree;
		delete [] numCNsEachDegree;

		inUse = false;

		return true;
	}
	else {
		return false;
	}
}

/* Load an edge list from a .txt file */
void EdgeList::loadTxt(const std::string &file_name){
	std::ifstream ifs(file_name, std::ios::in);
	std::string tmp;

	std::stringstream ss;
	std::stringstream trimmed_file_contents;

	int rowInd, colInd;
	bool usedRowInd = false;
	bool usedColInd = false;

	if (!ifs.is_open()){
		throw MatInpFileOpenFail(file_name);
	}

	if (inUse){
		reset();
	}

	inUse = true;

	while (getNextNBNCTLine(ifs, tmp, '#')){		
		ss = std::stringstream(tmp);

		if (usedRowInd && usedColInd){
			usedRowInd = false;
			usedColInd = false;
		}

		if (ss.rdbuf()->in_avail() == 0){
			continue;
		}

		if (!usedRowInd){
			ss >> rowInd;
			if (ss.fail() || ss.rdbuf()->in_avail() != 0){
				throw MatInpFileNonInteger{};
			}
			numCNs = (rowInd >= numCNs) ? (rowInd + 1) : numCNs;
			usedRowInd = true;
		}
		else {
			ss >> colInd;
			if (ss.fail() || ss.rdbuf()->in_avail() != 0){
				throw MatInpFileNonInteger{};
			}
			numVNs = (colInd >= numVNs) ? (colInd + 1) : numVNs;
			usedColInd = true;

			numEdges++;
		}
		trimmed_file_contents << ss.str() << " ";
	}

	/* Now that we have the number of VNs and CNs we can allocate memory for some
	 * dynamic arrays */

	CNConnections = new int[numEdges];
	VNConnections = new int[numEdges];

	CNDegrees = new int[numCNs];
	VNDegrees = new int[numVNs];

	CNConnectionInds = new int[2 * numCNs];
	VNConnectionInds = new int[2 * numVNs];

	int *CNNextInds = new int[numCNs];
	int *VNNextInds = new int[numVNs];

	/* End of memory allocation - for now! */

	for (int i = 0; i < numCNs; ++i){
		CNDegrees[i] = 0;
	}

	for (int i = 0; i < numVNs; ++i){
		VNDegrees[i] = 0;
	}	

	for (int i = 0; i < numEdges; ++i){
		trimmed_file_contents >> rowInd;
		trimmed_file_contents >> colInd;
		CNDegrees[rowInd]++;
		VNDegrees[colInd]++;
	}

	int currentDeg;
	int currentInd;

	/* Find the degree of each VN and the maximum and minimum VN degrees, and
	 * find the indices of the elements of ColEconnections that relate to each
	 * VN
	 */

	currentInd = 0;

	maxVNDeg = VNDegrees[0];
	minVNDeg = VNDegrees[0];

	for (int i = 0; i < numVNs; ++i){
		currentDeg = VNDegrees[i];
		if (currentDeg == 0){
			throw UnusedVN(i, numVNs);
		}
		maxVNDeg = (currentDeg > maxVNDeg) ? currentDeg : maxVNDeg;
		minVNDeg = (currentDeg < minVNDeg) ? currentDeg : minVNDeg;
		VNConnectionInds[2 * i] = currentInd;
		VNConnectionInds[2 * i + 1] = currentInd + currentDeg - 1;
		VNNextInds[i] = currentInd;
		currentInd += currentDeg;
	}

	/* Count the number of VNs of each degree */
	numVNsEachDegree = new int[maxVNDeg + 1];

	for (int i = 0; i < maxVNDeg; ++i){
		numVNsEachDegree[i] = 0;
	}

	for (int i = 0; i < numVNs; ++i){
		currentDeg = VNDegrees[i];
		numVNsEachDegree[currentDeg]++;
	}		

	/* Find the degree of each CN and the maximum and minimum CN degrees, and
	 * find the indices of the elements of ColEconnections that relate to each
	 * CN
	 */

	currentInd = 0;

	maxCNDeg = CNDegrees[0];
	minCNDeg = CNDegrees[0];

	for (int i = 0; i < numCNs; ++i){
		currentDeg = CNDegrees[i];
		if (currentDeg == 0){
			throw UnusedCN(i, numCNs);
		}
		maxCNDeg = (currentDeg > maxCNDeg) ? currentDeg : maxCNDeg;
		minCNDeg = (currentDeg < minCNDeg) ? currentDeg : minCNDeg;
		CNConnectionInds[2 * i] = currentInd;
		CNConnectionInds[2 * i + 1] = currentInd + currentDeg - 1;
		CNNextInds[i] = currentInd;
		currentInd += currentDeg;
	}

	/* Count the number of CNs of each degree */
	numCNsEachDegree = new int[maxCNDeg + 1];

	for (int i = 0; i < maxCNDeg; ++i){
		numCNsEachDegree[i] = 0;
	}

	for (int i = 0; i < numCNs; ++i){
		currentDeg = CNDegrees[i];
		numCNsEachDegree[currentDeg]++;
	}

	int nextInd;
	
	trimmed_file_contents.clear();
	trimmed_file_contents.seekg(0, trimmed_file_contents.beg);

	for (int i = 0; i < numEdges; ++i){
		trimmed_file_contents >> rowInd;
		trimmed_file_contents >> colInd;

		nextInd = CNNextInds[rowInd]++;
		CNConnections[nextInd] = colInd;

		nextInd = VNNextInds[colInd]++;
		VNConnections[nextInd] = rowInd;
	}

	delete [] VNNextInds;
	delete [] CNNextInds;
}

/* Load an edge list from a binary file */
void EdgeList::loadBin(const std::string &file_name){
	std::ifstream ifs(file_name, std::ios::binary | std::ios::in); // Output file stream

	/* If there was a problem opening the file, throw an exception */
	if (!ifs.is_open()){
		throw MatInpFileOpenFail(file_name);
	}

	if (inUse){
		reset();
	}

	inUse = true;

	int startInd, endInd; // Stores the start/end indices of the elements of CNConnections relevant to a given CN
	int CN; // Stores a CN number	
	int VN; // Stores a VN number
	int degree; // Stores the degree of a CN or VN;

	int fileVersion;

	size_t int_size = sizeof(int);

	/* First of all, we check the version number of the file format */
	ifs.read(reinterpret_cast<char*>(&fileVersion), int_size);
	if (fileVersion != (int)EDGE_LIST_BIN_FILE_VERSION){
		throw CantReadFileVersion{ };
	}

	/* Next, we read the total number of CNs, VNs and edges from the file */
	ifs.read(reinterpret_cast<char*>(&numCNs), int_size);
	ifs.read(reinterpret_cast<char*>(&numVNs), int_size);
	ifs.read(reinterpret_cast<char*>(&numEdges), int_size);

	CNConnections = new int[numEdges];
	VNConnections = new int[numEdges];

	CNConnectionInds = new int[2 * numCNs];
	VNConnectionInds = new int[2 * numVNs];

	int *CNNextInds = new int[numCNs];
	int *VNNextInds = new int[numVNs];

	numCNsEachDegree = new int[maxCNDeg];

	for (int i = 0; i < maxCNDeg; ++i){
		numCNsEachDegree[i] = 0;
	}

	numVNsEachDegree = new int[maxVNDeg];

	for (int i = 0; i < maxVNDeg; ++i){
		numVNsEachDegree[i] = 0;
	}

	CNDegrees = new int[numCNs];

	/* Next we read the CN degrees and find the maximum and minimum */
	CN = 0;
	startInd = 0;
	ifs.read(reinterpret_cast<char*>(&degree), int_size);
	CNDegrees[CN] = degree;
	maxCNDeg = degree;
	minCNDeg = degree;
	numCNsEachDegree[degree]++;
	endInd = startInd + degree - 1;
	CNConnectionInds[2 * CN] = startInd;
	CNConnectionInds[2 * CN + 1] = endInd;
	CNNextInds[CN] = startInd;
	startInd = endInd + 1;

	for (CN = 1; CN < numCNs; ++CN){
		ifs.read(reinterpret_cast<char*>(&degree), int_size);
		CNDegrees[CN] = degree;
		maxCNDeg = (maxCNDeg > degree) ? maxCNDeg : degree;
		minCNDeg = (minCNDeg < degree) ? minCNDeg : degree;
		numCNsEachDegree[degree]++;
		endInd = startInd + degree - 1;
		CNConnectionInds[2 * CN] = startInd;
		CNConnectionInds[2 * CN + 1] = endInd;
		CNNextInds[CN] = startInd;
		startInd = endInd + 1;
	}
	/* Done reading the CN degrees */

	VNDegrees = new int[numVNs];

	/* Next we read the CN degrees and find the maximum and minimum */
	VN = 0;
	startInd = 0;
	ifs.read(reinterpret_cast<char*>(&degree), int_size);
	VNDegrees[VN] = degree;
	maxVNDeg = degree;
	minVNDeg = degree;
	numVNsEachDegree[degree]++;
	endInd = startInd + degree - 1;
	VNConnectionInds[2 * VN] = startInd;
	VNConnectionInds[2 * VN + 1] = endInd;
	VNNextInds[VN] = startInd;
	startInd = endInd + 1;

	for (VN = 1; VN < numVNs; ++VN){
		ifs.read(reinterpret_cast<char*>(&degree), int_size);
		VNDegrees[VN] = degree;
		maxVNDeg = (maxVNDeg > degree) ? maxVNDeg : degree;
		minVNDeg = (minVNDeg < degree) ? minVNDeg : degree;
		numVNsEachDegree[degree]++;
		endInd = startInd + degree - 1;
		VNConnectionInds[2 * VN] = startInd;
		VNConnectionInds[2 * VN + 1] = endInd;
		VNNextInds[VN] = startInd;
		startInd = endInd + 1;
	}	
	/* Done reading the VN degrees */

	int VNInd, CNInd;

	for (int edge = 0; edge < numEdges; ++edge){
		ifs.read(reinterpret_cast<char*>(&CN), int_size);
		ifs.read(reinterpret_cast<char*>(&VN), int_size);

		CNInd = CNNextInds[CN];
		VNInd = VNNextInds[VN];

		CNConnections[CNInd] = VN;
		VNConnections[VNInd] = CN;

		CNNextInds[CN]++;
		VNNextInds[VN]++;
	}

	delete [] CNNextInds;
	delete [] VNNextInds;
}

/* Save an edge list to a .txt file */
void EdgeList::saveTxt(const std::string &file_name){
	std::ofstream ofs(file_name, std::ios::out); // Output file stream

	/* If there was a problem opening the file, throw an exception */
	if (!ofs.is_open()){
		throw MatOutFileOpenFail(file_name);
	}

	int startInd, endInd; // Stores the start/end indices of the elements of CNConnections relevant to a given CN
	int VN; // Stores a VN number

	/* Loop through each of the CNs */
	for (int CN = 0; CN < numCNs; ++CN){
		/* For each CN, find the indices in CNConnections that tell us the VNs
		 * the CN is connected to */
		CNConnInds(CN, startInd, endInd);

		/* Now use these indices to find the VNs that the CN is connected to
		 * and write each CN/VN pair to the output stream */
		for (int currentInd = startInd; currentInd <= endInd; ++currentInd){
			VN = CNConnections[currentInd];
			ofs << std::to_string(CN) << "\n" << std::to_string(VN) << "\n";
		}
	}

	/* Write the contents of the output stream to the file */
	ofs.flush();

	/* Close the file */
	ofs.close();
}

/* Save an edge list to a binary file - NB this is probably only sensible when there is a very large edge list */
void EdgeList::saveBin(const std::string &file_name){
	std::ofstream ofs(file_name, std::ios::binary | std::ios::out); // Output file stream

	/* If there was a problem opening the file, throw an exception */
	if (!ofs.is_open()){
		throw MatOutFileOpenFail(file_name);
	}

	int startInd, endInd; // Stores the start/end indices of the elements of CNConnections relevant to a given CN
	int CN; // Stores a CN number	
	int VN; // Stores a VN number
	int degree; // Stores the degree of a CN or VN;

	int fileVersion = EDGE_LIST_BIN_FILE_VERSION;

	size_t int_size = sizeof(int);

	/* First of all, we write a version number for the file format to the file */
	ofs.write(reinterpret_cast<char*>(&fileVersion), int_size);

	/* Next, we write the total number of CNs, VNs and edges to the file */
	ofs.write(reinterpret_cast<char*>(&numCNs), int_size);
	ofs.write(reinterpret_cast<char*>(&numVNs), int_size);
	ofs.write(reinterpret_cast<char*>(&numEdges), int_size);

	/* Next, we write the degrees of the CNs and VNs to the file */
	for (CN = 0; CN < numCNs; ++CN){
		degree = CNDegree(CN);
		ofs.write(reinterpret_cast<char*>(&degree), int_size);
	}

	for (VN = 0; VN < numVNs; ++VN){
		degree = VNDegree(VN);
		ofs.write(reinterpret_cast<char*>(&degree), int_size);
	}	

	/* Loop through each of the CNs */
	for (CN = 0; CN < numCNs; ++CN){
		/* For each CN, find the indices in CNConnections that tell us the VNs
		 * the CN is connected to */
		CNConnInds(CN, startInd, endInd);

		/* Now use these indices to find the VNs that the CN is connected to
		 * and write each CN/VN pair to the output stream */
		for (int currentInd = startInd; currentInd <= endInd; ++currentInd){
			VN = CNConnections[currentInd];
			ofs.write(reinterpret_cast<char*>(&CN), int_size);
			ofs.write(reinterpret_cast<char*>(&VN), int_size);
		}
	}

	/* Write the contents of the output stream to the file */
	ofs.flush();

	/* Close the file */
	ofs.close();
}

/* Skip comments (every line beginning with the character ch) in an input text 
 * file 
 */
unsigned int EdgeList::skipCommentLines(std::ifstream &ifs1, const char ch){
	std::streampos ifs2;

	std::string str;
	size_t ind;

	unsigned int ret = 0;

	while (true){
		ifs2 = ifs1.tellg();
		if (!getline(ifs1, str)){
			break;
		}
		ind = str.find(ch);

		if (ind == 0){
			ret++;			
		}
		else {
			ifs1.seekg(ifs2);
			break;
		}
	}

	return ret;
}

/* Skip blank lines and comment lines (every line beginning with the character ch)
 * in an input text file 
 */
unsigned int EdgeList::skipBlankCommentLines(std::ifstream &ifs1, const char ch){
	std::streampos ifs2;

	std::string str;
	size_t ind;

	unsigned int ret = 0;

	while (true){
		ifs2 = ifs1.tellg();
		if (!getline(ifs1, str)){
			break;
		}
		trimRWSpace(str);
		ind = str.find(ch);

		if (ind == 0 || str.length() == 0){
			ret++;			
		}
		else {
			ifs1.seekg(ifs2);
			break;
		}
	}

	return ret;
}

/* Remove comments (everything including and after the first appearance of the character
 * ch) from a string 
 */
std::string EdgeList::trimComments(const std::string &str, const char ch){
	std::string str_out;

	size_t ind = str.find(ch);

	str_out = str.substr(0, ind-1);

	return str_out;
}

/* Get the next non-blank (NB), non-comment (NC) line - Like getline() but skips lines 
 * that start with the comment character or that consist entirely of whitespace 
 */
std::istream& EdgeList::getNextNBNCLine(std::ifstream &ifs, std::string &str, const char ch){
	skipBlankCommentLines(ifs, ch);
	return getline(ifs, str);
}

/* Get the next non-blank (NB), non-comment (NC), trimmed (T) line - Like getNextNBNCLine() but 
 * trims any comments AND whitespace from the end of the resulting string
 */
std::istream& EdgeList::getNextNBNCTLine(std::ifstream &ifs, std::string &str, const char ch){
	// istream& ret;

	skipBlankCommentLines(ifs, ch);

	std::istream& ret = getline(ifs, str);

	str = trimComments(str, ch);

	trimRWSpace(str);

	return ret;
}