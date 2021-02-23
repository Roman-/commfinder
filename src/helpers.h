#ifndef SMC_HELPERS_H
#define SMC_HELPERS_H
#include <array>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include "easylogging++.h"

/// @returns file contents as string
/// @param quiet - don't yell on error
std::string getFileContents(std::string_view filename, bool quiet = false);

/// saves @param content to file
bool saveToFile(std::string_view  path, const std::string& content, bool append = false);

/// returns true if @param arr contains element @param elem
template<class T, std::size_t SIZE>
inline bool inArray(const std::array<T, SIZE>& arr, const T& elem) {
    return arr.cend() != std::find(arr.cbegin(), arr.cend(), elem);
}

// returns index of element @param elem in vector @param vec. If not found, returns -1
template <class T>
int indexOf(const std::vector<T>& vec, const T& elem) {
    const auto itr = std::find(vec.begin(), vec.end(), elem);
    return (vec.cend() == itr) ? -1 : std::distance(vec.begin(), itr);
}

/// returns number of elements that differ on the same positions in arrays
template<std::size_t SIZE>
uint8_t numMismatches(const std::array<uint8_t, SIZE>& v1, const std::array<uint8_t, SIZE>& v2) {
    uint8_t res = 0;
    for (uint8_t i = 0; i < SIZE; ++i)
        if (v1[i] != v2[i])
            ++res;
    return res;
}

/// split string by character @param c to vector of strings
std::vector<std::string> splitString(const std::string inputString, char c);

/// join to single string by character @param c
std::string joinStrings(const std::vector<std::string>& strings, char c);

inline std::chrono::time_point<std::chrono::steady_clock> now() {
    return std::chrono::steady_clock::now();
}

template<size_t SIZE>
std::string arrToString(const std::array<uint8_t, SIZE>& state) {
    std::string result;
    for (size_t i = 0; i < SIZE; ++i)
        result += (i?", ":"") + std::to_string(state[i]);
    return "[" + result + "]";
}

// returns true if array with elements [0,size) has all its elements grouped,
// e.g. for group size = 4,
// [0,1,2,3,5,4,6,7,8,9,10,11] // this is grouped, because (4,5,6,7) elems are in their group
// |  G1   |  G2   |    G3   |
// [0,1,2,3,4,5,6,7,8,9,10,11] < indeces
template<size_t SIZE>
bool areGroupedBy(uint8_t groupSize, const std::array<uint8_t, SIZE>& state) {
    for (uint8_t i = 0; i < SIZE; ++i) {
        uint8_t groupMin = i - i%groupSize;
        uint8_t groupMax = i - i%groupSize + groupSize;
        if (state[i] < groupMin || state[i] >= groupMax) {
            return false;
        }
    }
    return true;
}

// same as areGroupedBy but returns false if they're sorted
template<size_t SIZE>
bool areGroupedByButNotSorted(uint8_t groupSize, const std::array<uint8_t, SIZE>& state) {
    bool isSorted = true;
    for (uint8_t i = 0; i < SIZE; ++i) {
        if (i>0 && isSorted)
            isSorted &= (state[i-1] < state[i]);
        uint8_t groupMin = i - i%groupSize;
        uint8_t groupMax = i - i%groupSize + groupSize;
        if (state[i] < groupMin || state[i] >= groupMax) {
            return false;
        }
    }
    return !isSorted;
}

/// @returns true if there's at least one group that has all group's elements
/// but all of them are out of their positions. E.g. |012|345| - false, |120|345| - true
template<size_t SIZE>
bool hasGroupedByButNotSorted(uint8_t groupSize, const std::array<uint8_t, SIZE>& state) {
    bool has = false;
    for (uint8_t i = 0; i < SIZE; ++i) {
        if (i%groupSize == 0) {
            // New group begins. If previous group was exactly as expected, return true
            if (has) {
                return true;
            }
            has = true; // assumption
        }
        uint8_t groupMin = i - i%groupSize;
        uint8_t groupMax = i - i%groupSize + groupSize;
        if (state[i] < groupMin || state[i] >= groupMax || state[i] == i) {
            has = false;
        }
    }
    return has;
}

#endif // SMC_HELPERS_H
