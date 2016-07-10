#ifndef VECTOR_TYPES_HPP
#define VECTOR_TYPES_HPP

#include <vector>

typedef std::vector<float> BPSKTXVector; // A vector that has ben modulated for transmission using BPSK
typedef std::vector<float> BPSKRXVector; // A BPSK-modulated vector that has been received and decoded
typedef std::vector<float> BECRXVector;
typedef std::vector<float> BECBPSKRXVector;
typedef std::vector<unsigned int> ErrorIndVector;

#endif