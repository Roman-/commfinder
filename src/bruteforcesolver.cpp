#include "bruteforcesolver.h"
#include <easylogging++.h>

BruteForceSolver::BruteForceSolver(const CubeState &stateToSolve):
    initialState_(stateToSolve)
{
}

MovesArray BruteForceSolver::solve(uint8_t maxSolutionLength) {
    static MovesArray none = emptyMovesArray();
    if (initialState_.isSolved()) {
        LOG(WARNING) << "BruteForceSolver::solve but state is already solved, returning 0";
        return none;
    }

    bool isSolved = false;

    // TODO introduce parallelism
    CubeState state = initialState_;
    uint8_t currentDepth = 1;
    // TODO measure time and report progress
    while (currentDepth <= maxSolutionLength) {
        state = initialState_; // TODO compare this with undoing moves
        state.applyScramble(iterativeScramble_.get());
        isSolved |= state.isSolved();
        if (isSolved) {
            LOG(INFO) << "found solution: " << iterativeScramble_.toString();
            break;
        }

        if (iterativeScramble_.size() != currentDepth) {
            currentDepth = iterativeScramble_.size();
            LOG_IF(currentDepth > 2, INFO) << "searching depth " << int(currentDepth);
        }
        ++iterativeScramble_;
    }

    return isSolved ? iterativeScramble_.get() : none;
}
