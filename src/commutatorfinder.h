#ifndef COMMUTATORFINDER_H
#define COMMUTATORFINDER_H
#include "incrementalscramble.h"
#include "cubestate.h"
#include "searchcriteria.h"

#include <string>
#include <chrono>

// Breadth-first searches through all possible commutators and saves search results to a file.
// Commutator is [A, B] = A B A' B' where part A is a single move and part B size <= maxMovespartB
class CommutatorFinder {
public:
    /// @param maxMovespartB - partB moves count limit
    /// @param outputFilePath - either /path/to/outputAllComs.txt or /path/to/outputdir/
    /// If a directory specified, results of each type will be saved to a separate file
    CommutatorFinder(uint8_t maxMovesPartB, const SearchCriteria& criteria
                     , std::string_view outputPath);

    /// Starts search in a busy-loop
    /// @returns number of commutators found
    uint64_t find();

    /// convert currently founded scramble to string
    /// @param commutatorNotation - if true, write scramble in commutator notation
    /// otherwise will be returned as a full sequence of space-separated moves
    std::string toString(bool commutatorNotation = true) const;

    // reset partA, partB
    void reset();

private:
    uint8_t partA_;
    IncrementalScramble partB_;
    uint8_t maxMovesPartB_;
    SearchCriteria criteria_;

    // either a .txt file path or directory path
    std::string outputPath_;

    // total number of results
    uint64_t numResults_;

    // if true, each result will be saved in separate file
    bool outputToDir_;

    // return path to output file to output special cases
    std::string pathToOutFile(CaseType ct) const;

    // prints message of finished search
    void printFinishMessage() const;

    // prints message of finished all partB algs for current partA
    void printPartAdoneMessage() const;

    // prints progress message if, but no more than once in N seconds
    void printOccasionalProgressReport() const;

    // found commutator result => save to file and increment numResults
    void onFoundResult(CaseType caseType, CubeState& cube);

    // last time we printed non-critical logging message. This us for occasional progress only
    mutable std::chrono::time_point<std::chrono::steady_clock> lastLogging_;

    // for logging
    mutable uint64_t lastResultPartA_;
};


#endif // COMMUTATORFINDER_H
