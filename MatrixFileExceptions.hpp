#ifndef MATRIX_FILE_EXCEPTIONS
#define MATRIX_FILE_EXCEPTIONS

#include <exception>
#include <string>

class MatInpFileOpenFail{
	public:
		MatInpFileOpenFail(std::string str){
			fileName = str;
		}
		virtual const char* what() const throw(){
			std::string msg = "The file " + fileName + " could not be opened.\n";
			return msg.c_str();
		}	
	private:
		std::string fileName;
};

class MatOutFileOpenFail{
	public:
		MatOutFileOpenFail(std::string str){
			fileName = str;
		}
		virtual const char* what() const throw(){
			std::string msg = "The file " + fileName + " could not be opened.\n";
			return msg.c_str();
		}	
	private:
		std::string fileName;
};

class MatInpFileNonInteger{
	public:
		// MatInpFileOpenFail(std::string str){
		// 	fileName = str;
		// }
		virtual const char* what() const throw(){
			std::string msg = "The input file must contain one integer value per line.\n";
			return msg.c_str();
		}	
	private:
		std::string fileName;
};

class MatInpFileTooFewCols{
	public:
		MatInpFileTooFewCols(std::string str, const unsigned int currentRow, const unsigned int currentCol, const unsigned int expectedCols){
			fileName = str;
			row = currentRow;
			col = currentCol;
			expCols = expectedCols;
		}
		virtual const char* what() const throw(){
			std::string msg = "The file " + fileName + " has too many column elements (" + std::to_string(col-1) + ") in row " + std::to_string(row) + " (expected " + std::to_string(expCols) + " columns).\n";
			return msg.c_str();
		}	
	private:
		unsigned int row, col, expCols;
		std::string fileName;
};

class MatInpFileTooManyCols{
	public:
		MatInpFileTooManyCols(std::string str, const unsigned int currentRow, const unsigned int currentCol, const unsigned int expectedCols){
			fileName = str;
			row = currentRow;
			col = currentCol;
			expCols = expectedCols;
		}
		virtual const char* what() const throw(){
			std::string msg = "The file " + fileName + " has too few column elements (" + std::to_string(col-1) + ") in row " + std::to_string(row) + " (expected " + std::to_string(expCols) + " columns).\n";
			return msg.c_str();
		}	
	private:
		unsigned int row, col, expCols;
		std::string fileName;
};


#endif