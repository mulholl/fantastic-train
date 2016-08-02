#ifndef FANTASTIC_TRAIN_HPP
#define FANTASTIC_TRAIN_HPP

#include "fantastic-train-config.hpp"

// #include <Eigen/Dense>
// #include <Eigen/SparseCore>
// #include <Eigen/SparseExtra>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <list>

// #include "MatrixFileFunctions.hpp"
#include "Modulators.hpp"
#include "BEChannel.hpp"
#include "BECDecoder.hpp"
#include "Codewords.hpp"
#include "InpOpts.hpp"
#include "FantasticTrainMainFunctions.hpp"
#include "FantasticTrainExceptions.hpp"
#include "ChannelModels.hpp"
#include "EdgeList.hpp"

#define CHANNEL_PARAM_MODE_SINGLE_VALUE 1
#define CHANNEL_PARAM_MULTI_LINE_RANGE_OF_VALUES 2
#define CHANNEL_PARAM_ONE_LINE_RANGE_OF_VALUES 3
#define CHANNEL_PARAM_LIST_OF_VALUES 4

#define CHANNEL_TYPE_BEC 1
// #define CHANNEL_TYPE_AWGN 2

#define EXIT_BAD_INPUTS -1
#define EXIT_FILE_FAILURE -2

#endif