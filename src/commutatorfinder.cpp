#include "commutatorfinder.h"
#include "helpers.h"
#include "cube_moves.h"
#include <easylogging++.h>
#include <thread>
using namespace std::chrono_literals;

CommutatorFinder::CommutatorFinder(uint8_t maxMovesPartB
                                   , const SearchCriteria& criteria
                                   , std::string_view outputPath):
    maxMovesPartB_(maxMovesPartB)
  , criteria_(criteria)
  , outputPath_(outputPath)
  , numResults_(0)
  , lastLogging_(now())
  , lastResultPartA_(0)
{
    LOG_IF(outputPath_.empty(), FATAL) << "empty outputPath";
    outputToDir_ = ('/' == outputPath_.back());
    reset();
}

uint64_t CommutatorFinder::find() {
    reset();
    LOG(INFO) << "Begin commutators search. Max partB = " << int(maxMovesPartB_) << " moves. "
              << "Results will be saved to "
              << (outputToDir_ ? (outputPath_+"*.txt") : outputPath_);
    uint32_t count = 0; // for occasional logging
    uint8_t i;
    while (true) {
        CubeState state;
        // apply commutator
        const MovesArray& moves = partB_.get();
        i = 0;
        // apply A B A' B'
        state.applyScrambleMove(partA_);
        while (i < moves.size() && moves[i] != kNoMove)
            state.applyScrambleMove(moves[i++]);
        state.applyScrambleMove(oppoMove(partA_));
        while (i>0)
            state.applyScrambleMove(oppoMove(moves[--i]));

        // see if we've found something interesting
        CaseType ct = state.getCaseType(criteria_);
        if (CaseType::caseTypeEnd != ct)
            onFoundResult(ct, state);

        // increment partB_
        partB_.incAndSkipParallelBeginEnd(partA_);

        // report progress based on time
        if (++count%1000 == 0)
            printOccasionalProgressReport();

        if (partB_.size() > maxMovesPartB_) {
            partB_.reset();
            partA_++;
            if (partA_ >= kNumAllQtmMoves) {
                printFinishMessage();
                return numResults_;
            } else {
                printPartAdoneMessage();
            }
        }
    }
}

std::string CommutatorFinder::toString(bool commutatorNotation) const {
    if (commutatorNotation)
        return "[" + moveToString(partA_) + ", " + partB_.toString() + "]";
    std::string fullScramble = moveToString(partA_) + " " + partB_.toString() + " "
                + moveToString(oppoMove(partA_));
    for (int i = partB_.size() - 1; i >= 0; --i)
        fullScramble += " " + moveToString(oppoMove(partB_.get()[i]));
    return fullScramble;
}

void CommutatorFinder::reset() {
    numResults_ = 0;
    partA_ = uint8_t(0);
    partB_.reset();
    lastLogging_ = now();
    lastResultPartA_ = 0;

    // check if output files(s) available; create/clear them
    if (outputToDir_) {
        for (size_t i = 0; i < size_t(CaseType::caseTypeEnd); ++i) {
            CaseType ct = static_cast<CaseType>(i);
            std::string filePath = pathToOutFile(ct);
            auto contents = getFileContents(filePath, true);
            LOG_IF(!contents.empty() && !saveToFile(filePath, "", false), FATAL)
                    << "can\'t open file " << filePath << " for writing";
        }
    } else {
        bool fileIsOk = saveToFile(outputPath_, "", false);
        LOG_IF(!fileIsOk, FATAL) << "Can\'t write to file: " << outputPath_;
    }
}

void CommutatorFinder::printFinishMessage() const {
    LOG(INFO) << "Found " << numResults_ << " commutators [A, B] where B is up to "
              << int(maxMovesPartB_) << " moves. Results saved to " << outputPath_;
}

void CommutatorFinder::printPartAdoneMessage() const {
    uint64_t resultsForPartB = numResults_ - lastResultPartA_;
    lastResultPartA_ = numResults_;
    LOG(INFO) << "Finished partA = "
              << moveToString(partA_-1) << ", found " << resultsForPartB
              << " comms with " << int(maxMovesPartB_)
              << "-move-partB. Total comms found: " << numResults_;
    lastLogging_ = now();
}

void CommutatorFinder::onFoundResult(CaseType caseType, CubeState& cube) {
    // TODO if the element of castType isn't located on some layers (e.g. corners aren't located
    // on layers M, l, b etc., then discard the alg if it has these layer moves
    ++numResults_;
    auto outputFilePath = outputToDir_
            ? pathToOutFile(caseType) : outputPath_;
    // cube.solveAndGetCycles() will print centers cycles as well. Replace with asterics
    std::string delimeter = ": ";
    if (!isCenterCaseType(caseType) && !cube.centersAreSolved()
            && CenterSafety::StrictCenterSafe != criteria_.getCenterSafety()) {
        cube.resetCenters();
        delimeter = "*" + delimeter;
    }
    std::string contents = cube.solveAndGetCycles() + delimeter + this->toString() + "\n";
    bool saved = false;
    do {
        saved = saveToFile(outputFilePath, contents, true);
        if (!saved) {
            LOG(ERROR) << "failed to save to " << outputFilePath << ". Retrying in 10s";
            std::this_thread::sleep_for(10s);
        }
    } while (!saved);
}

std::string CommutatorFinder::pathToOutFile(CaseType ct) const {
    return outputPath_ + ::toString(ct) + std::to_string(partB_.size()) + "moves.txt";
}

void CommutatorFinder::printOccasionalProgressReport() const {
    constexpr auto kLogProgressInterval = 5s;
    if (now() - lastLogging_ < kLogProgressInterval)
        return;
    LOG(INFO) << "progress: partA = " << moveToString(partA_) << " ("
              << int(1+partA_) << "/" << int(kNumAllQtmMoves) << " mv), partB "
              << int(partB_.size()) << "/" << int(maxMovesPartB_)
              << " mv = " << partB_.progress() << ". Total comms: " << numResults_;
    lastLogging_ = now();
}
