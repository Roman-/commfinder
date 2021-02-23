#include "incrementalscramble.h"
#include "easylogging++.h"

IncrementalScramble::IncrementalScramble() {
    reset();
}

void IncrementalScramble::reset() {
    std::fill(moves_.begin(), moves_.end(), kNoMove);
    moves_[0] = 0;
    size_ = 1;
}

IncrementalScramble& IncrementalScramble::operator++() {
    static bool hasRedundancy;

    do {
        hasRedundancy = false;
        ++moves_[0];
        for (uint8_t i = 0; i < size_ && moves_[i] >= kNumAllQtmMoves;) {
            moves_[i] = 0;
            if (i == size_ - 1) {
                moves_[i] = 0;
                moves_[size_++] = 0;
            } else {
                ++moves_[++i];
            }
        }

        // L U U' F -> increment U, reset everything to the left, decrement
        for (uint8_t j = 0; j < size_-1; ++j) {
            if (areMovesOfSameFace(moves_[j], moves_[j+1])) {
                hasRedundancy = true;
                break;
            }
        }

        // Remove "URL"="ULR" redundancy by placing parallel moves in ascending order
        if (!hasRedundancy) {
            for (uint8_t j = 0; j < size_-1; ++j) {
                if (areParallelLayersMoves(moves_[j], moves_[j+1])) {
                    hasRedundancy |= (baseMove(moves_[j]) >= baseMove(moves_[j+1]));
                    if (hasRedundancy) {
                        break;
                    }
        }   }   }
    } while (hasRedundancy);

    return *this;
}

IncrementalScramble &IncrementalScramble::incAndSkipParallelBeginEnd(uint8_t move) {
    this->operator++();
    bool firstMoveParallel(false), lastMoveParallel(false);
    do {
        lastMoveParallel = areParallelLayersMoves(moves_[size_-1], move);
        if (lastMoveParallel) {
            for (uint8_t j = 0; j < size_-1; ++j) {
                moves_[j] = kNumAllQtmMoves-1;
            }
            this->operator++();
        }
        firstMoveParallel = areParallelLayersMoves(moves_[0], move);
        if (firstMoveParallel)
            this->operator++();

    } while (firstMoveParallel || lastMoveParallel);

    return *this;
}

std::string IncrementalScramble::toString() const {
    return ::toString(moves_);
}

std::string IncrementalScramble::progress() const {
    if (size_ < 3)
        return "instant";
    constexpr double k1 = kNumAllQtmMoves, k2 = k1*k1, k3 = k1*k1*k1;
    constexpr double maxProgress = k3 + k2 + k1;
    double it = moves_[size_-1] * k2
              + moves_[size_-2] * k1
              + moves_[size_-3];
    std::string result = std::to_string(it/maxProgress*100.) + "%";
    return result;
}

const MovesArray &IncrementalScramble::get() const {
    return moves_;
}

std::size_t IncrementalScramble::size() const {
    return size_;
}
