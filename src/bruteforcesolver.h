#ifndef BRUTEFORCESOLVER_H
#define BRUTEFORCESOLVER_H

#include "incrementalscramble.h"
#include "cubestate.h"
#include "cube_moves.h"

// bruteforce solve specified CubeState, depth-first search
class BruteForceSolver {
public:
    BruteForceSolver(const CubeState& stateToSolve);
    MovesArray solve(uint8_t maxSolutionLength = kMaxScrambleLength);
private:
    CubeState initialState_;
    IncrementalScramble iterativeScramble_;
};

#endif // BRUTEFORCESOLVER_H
