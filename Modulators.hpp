#ifndef MODULATORS_HPP
#define MODULATORS_HPP

#include <vector>

#include "VectorTypes.hpp"
#include "Codewords.hpp"
#include "BEChannel.hpp"

BPSKTXVector BPSKMod(const BinTXVector &vecIn);
BECRXVector BPSKDemod(const BECBPSKRXVector &vecIn);

#endif