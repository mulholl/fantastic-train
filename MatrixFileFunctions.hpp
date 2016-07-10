#ifndef MATRIX_FILE_FUNCTIONS_HPP
#define MATRIX_FILE_FUNCTIONS_HPP

#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <locale>
#include <typeinfo>
#include "MatrixFileExceptions.hpp"
#include "StringFunctions.hpp"

bool loadSparseBinMatFromTxt(Eigen::SparseMatrix<bool> &mat, const std::string str);
bool saveSparseBinMatToTxt(const Eigen::SparseMatrix<bool> &mat, const std::string str);
bool readMatFromTxt(Eigen::MatrixXi &mat, const std::string str);
static unsigned int skipCommentLines(std::ifstream &ifs, const char ch);
static unsigned int skipBlankCommentLines(std::ifstream &ifs, const char ch);
static std::string trimComments(const std::string str, const char ch);
static std::istream& getNextNBNCLine(std::ifstream &ifs, std::string &str, const char ch);
static std::istream& getNextNBNCTLine(std::ifstream &ifs, std::string &str, const char ch);

#endif