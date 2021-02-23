#ifndef INCREMENTALSCRAMBLE_H
#define INCREMENTALSCRAMBLE_H
#include <string>
#include "cube_moves.h"

// IncrementalScramble starts with size=1 and state = {0,-1,-1,-1...} which is L move
// It increments by incrementing leftmost digits first: 000, 100, 200, ..., {44}00, 010, 110, ...
// It also skips redundant scrambles, e.g. ULL=100 will never happen, as well as "L L2" or "l M l'"
// Parallel subsequences in scrambles (e.g. R U D) is guaranteed to be sorted ascending:
// "R D U" won't be generated
class IncrementalScramble {
public:
    IncrementalScramble();

    /// next algorithm
    IncrementalScramble& operator++();

    /// increments and skips cases when first or last move is parallel to @param move
    IncrementalScramble& incAndSkipParallelBeginEnd(uint8_t move);

    /// \returns current alg as a sequence of moves
    std::string toString() const;

    /// \returns progress for current size_ - report string in percent
    std::string progress() const;

    /// \returns current scramble
    const MovesArray& get() const;

    /// resets the scramble
    void reset();

    /// \returns current alg size
    std::size_t size() const;
private:
    uint8_t size_;
    MovesArray moves_;
};

#endif // INCREMENTALSCRAMBLE_H
