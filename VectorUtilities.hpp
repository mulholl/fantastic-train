#ifndef VECTOR_UTILITIES_HPP
#define VECTOR_UTILITIES_HPP

#include <vector>
#include <algorithm>
#include <functional>

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(result), std::plus<T>());
    return result;
}

template <typename T>
std::vector<T> operator * (const std::vector<T>& a, const T b)
{
    std::vector<T> result;
    result.resize(a.size());

    std::transform(a.begin(), a.end(), result.begin(), std::bind2nd(std::multiplies<T>(), b));

    return result;
}

template <typename T>
std::vector<T> operator * (const int b, const std::vector<T>& a)
{
    std::vector<T> result;
    result.resize(a.size());

    std::transform(a.begin(), a.end(), result.begin(), std::bind2nd(std::multiplies<T>(), b));

    return result;
}

template <typename T>
std::vector<T> operator / (const std::vector<T>& a, const float b)
{
    std::vector<T> result;
    result.resize(a.size());

    std::transform(a.begin(), a.end(), result.begin(), std::bind2nd(std::divides<T>(), b));

    return result;
}

#endif