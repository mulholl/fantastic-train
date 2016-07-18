#include "MatrixFileFunctions.hpp"

using namespace std;
using namespace Eigen;

bool loadSparseBinMatFromTxt(SparseMatrix<bool> &mat, const string str){
	mat.resize(0, 0);

	vector< Triplet<bool> > vec;

	ifstream ifs(str, ios::in);
	string tmp;

	stringstream ss;

	unsigned int rowInd, colInd;
	unsigned int numRows = 0;
	unsigned int numCols = 0;
	bool usedRowInd = false;
	bool usedColInd = false;

	unsigned int count = 0;

	if (ifs.is_open()){
		// cout << "FILE IS OPEN!" << endl;
		while (getline(ifs, tmp)){
			// cout << "tmp = |" << tmp << "|" << endl;
			trimLWSpace(tmp);
			trimRWSpace(tmp);

			ss = stringstream(tmp);

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
				numRows = (rowInd >= numRows) ? (rowInd + 1) : numRows;
				usedRowInd = true;
			}
			else {
				ss >> colInd;
				if (ss.fail() || ss.rdbuf()->in_avail() != 0){
					throw MatInpFileNonInteger{};
				}
				numCols = (colInd >= numCols) ? (colInd + 1) : numCols;
				usedColInd = true;

				vec.push_back(Triplet<bool>(rowInd, colInd, true));
				// ++count;
			}
		}

		mat.resize(numRows, numCols);

		// mat2.coeffRef(rowInd, colInd) = true;
		mat.setFromTriplets(vec.begin(), vec.end());

		// cout << "Done\n\n\n" << endl;
	}
	else {
		throw MatInpFileOpenFail(str);
	}

	return true;
}

bool loadSparseBinMatFromTxt(vector< vector<unsigned int> > &rowMajor, vector< vector<unsigned int> > &colMajor, const std::string str){
	rowMajor.clear();
	colMajor.clear();

	ifstream ifs(str, ios::in);
	string tmp;

	stringstream ss;

	unsigned int rowInd, colInd;
	unsigned int numRows = 0;
	unsigned int numCols = 0;
	bool usedRowInd = false;
	bool usedColInd = false;

	unsigned int count = 0;

	if (ifs.is_open()){
		
		while (getline(ifs, tmp)){
			// cout << "tmp = |" << tmp << "|" << endl;
			trimLWSpace(tmp);
			trimRWSpace(tmp);

			ss = stringstream(tmp);

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
				numRows = (rowInd >= numRows) ? (rowInd + 1) : numRows;
				usedRowInd = true;
			}
			else {
				ss >> colInd;
				if (ss.fail() || ss.rdbuf()->in_avail() != 0){
					throw MatInpFileNonInteger{};
				}
				numCols = (colInd >= numCols) ? (colInd + 1) : numCols;
				usedColInd = true;

				// vec.push_back(Triplet<bool>(rowInd, colInd, true));
				// ++count;
			}
		}

		// ifs.clear();
		// ifs.seekg(0, ifs.beg);

		// vector<unsigned int> rowDegrees(numRows, 0);
		// vector<unsigned int> colDegrees(numCols, 0);

		// vector<unsigned int>::iterator rowDegreeIt;
		// vector<unsigned 

		// while (getline(ifs, tmp)){
		// 	// cout << "tmp = |" << tmp << "|" << endl;
		// 	trimLWSpace(tmp);
		// 	trimRWSpace(tmp);

		// 	ss = stringstream(tmp);

		// 	if (usedRowInd && usedColInd){
		// 		usedRowInd = false;
		// 		usedColInd = false;
		// 	}

		// 	if (ss.rdbuf()->in_avail() == 0){
		// 		continue;
		// 	}

		// 	if (!usedRowInd){
		// 		ss >> rowInd;
		// 		if (ss.fail() || ss.rdbuf()->in_avail() != 0){
		// 			throw MatInpFileNonInteger{};
		// 		}
		// 		rowDegreeIt = rowDegrees.begin() + rowInd;
		// 		(*rowDegreeIt)++;
		// 		usedRowInd = true;
		// 	}
		// 	else {
		// 		ss >> colInd;
		// 		if (ss.fail() || ss.rdbuf()->in_avail() != 0){
		// 			throw MatInpFileNonInteger{};
		// 		}
		// 		colDegreeIt = colDegrees.begin() + colInd;
		// 		(*colDegreeIt)++;
		// 		usedColInd = true;

		// 		// vec.push_back(Triplet<bool>(rowInd, colInd, true));
		// 		// ++count;
		// 	}
		// }int>::iterator colDegreeIt;
		rowMajor.resize(numRows);
		colMajor.resize(numCols);

		vector< vector<unsigned int> >::iterator rowMajorIt;
		vector< vector<unsigned int> >::iterator colMajorIt;

		// for (rowDegreeIt = rowDegrees.begin(); rowDegreeIt < rowDegrees.end(); ++rowDegreeIt){
		// 	rowMajorIt = rowMajor.begin() + distance(rowDegrees.begin(), rowDegreeIt);
		// 	(*rowMajorIt).resize(*rowDegreeIt);
		// }

		// for (colDegreeIt = colDegrees.begin(); colDegreeIt < colDegrees.end(); ++colDegreeIt){
		// 	colMajorIt = colMajor.begin() + distance(colDegrees.begin(), colDegreeIt);
		// 	(*colMajorIt).resize(*colDegreeIt);
		// }

		ifs.clear();
		ifs.seekg(0, ifs.beg);

		while (getline(ifs, tmp)){
			trimLWSpace(tmp);
			trimRWSpace(tmp);

			ss = stringstream(tmp);

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
				rowMajorIt = rowMajor.begin() + rowInd;
				usedRowInd = true;
			}
			else {
				ss >> colInd;
				if (ss.fail() || ss.rdbuf()->in_avail() != 0){
					throw MatInpFileNonInteger{};
				}
				colMajorIt = colMajor.begin() + colInd;
				usedColInd = true;

				(*rowMajorIt).push_back(colInd);
				(*colMajorIt).push_back(rowInd);
			}
		}
	}
	else {
		throw MatInpFileOpenFail(str);
	}

	return true;
}

void saveSparseBinMatToTxt(const SparseMatrix<bool> &mat, const string str){
	ofstream ofs(str, ios::out);

	if (ofs.is_open()){
		for (int k = 0; k < mat.outerSize(); ++k){
			for (SparseMatrix<bool>::InnerIterator it(mat, k); it; ++it){
				ofs << it.row() << "\n" << it.col() << "\n";
			}
		}
	}
	else {
		throw MatOutFileOpenFail(str);
	}
}

void readMatFromTxt(MatrixXi &mat, const string str){
	ifstream ifs(str, ios::in);

	string tmp;
	stringstream ss;

	unsigned int numCols = 0;
	unsigned int numRows = 0;
	unsigned int currentCol = 0;
	unsigned int currentRow = 0;

	int tmp_int;

	if (ifs.is_open()){
		/* First check the file to figure out how large the matrix is */
		if (getNextNBNCTLine(ifs, tmp, '#')){
			// tmp = trimComments(tmp, '#');
			// trimRWSpace(tmp);
			ss = stringstream(tmp);

			/* Figure out the number of columns */
			while (ss >> tmp_int){
				numCols++;
				if (ss.rdbuf()->in_avail() == 0){
					break;
				}
			}
			/* Check whether the while loop terminated due to finding a non-integer value
			 * If so we have a problem and throw an exception */
			if (ss.fail()){
				throw MatInpFileNonInteger{};
			}

			/* Now figure out the number of rows */
			numRows++;
			while (getNextNBNCTLine(ifs, tmp, '#')){
				numRows++;
			}

			/* Now that we have the number of rows and columns, we can set the size of the
			 * matrix */
			mat.resize(numRows, numCols);
			mat.setZero();
		}

		cout << "mat.rows() = " << mat.rows() << endl;
		cout << "mat.cols() = " << mat.cols() << endl;

		ifs.clear();
		ifs.seekg(0, ifs.beg);

		while (getNextNBNCTLine(ifs, tmp, '#')){
			cout << "tmp = " << tmp << endl;
			ss = stringstream(tmp);

			for (unsigned int currentCol = 0; currentCol < numCols; ++currentCol){
				if (ss.rdbuf()->in_avail() > 0){
					ss >> mat(currentRow, currentCol);

					if (ss.fail()){
						throw MatInpFileNonInteger{};
					}
				}
				else {
					/* If we arrive here then this line has fewer entries than expected */
					throw MatInpFileTooFewCols(str, currentRow, currentCol, numCols);
				}
			}

			if (ss.rdbuf()->in_avail() > 0){
				/* If we arrive here then this line has more entries than expected */
				ss >> tmp_int;
				if (ss.fail()){
					throw MatInpFileNonInteger{};
				}
				else {
					throw MatInpFileTooManyCols(str, currentRow, currentCol, numCols);
				}
			}

			currentRow++;
		}
		cout << "READ THE MATRIX! mat = \n" << mat << endl;
	}
	else {
		throw MatInpFileOpenFail(str);
	}
}

unsigned int skipCommentLines(ifstream &ifs1, const char ch){
	streampos ifs2;

	string str;
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

unsigned int skipBlankCommentLines(ifstream &ifs1, const char ch){
	streampos ifs2;

	string str;
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

string trimComments(const string str, const char ch){
	string str_out;

	size_t ind = str.find(ch);

	str_out = str.substr(0, ind-1);

	return str_out;
}

/* Get the next non-blank (NB), non-comment (NC) line - Like getline() but skips lines 
 * that start with the comment character or that consist entirely of whitespace 
 */
istream& getNextNBNCLine(ifstream &ifs, string &str, const char ch){
	skipBlankCommentLines(ifs, ch);
	return getline(ifs, str);
}

/* Get the next non-blank (NB), non-comment (NC), trimmed (T) line - Like getNextNBNCLine() but 
 * trims any comments AND whitespace from the end of the resulting string
 */
istream& getNextNBNCTLine(ifstream &ifs, string &str, const char ch){
	// istream& ret;

	skipBlankCommentLines(ifs, ch);

	istream& ret = getline(ifs, str);

	str = trimComments(str, ch);

	trimRWSpace(str);

	return ret;
}