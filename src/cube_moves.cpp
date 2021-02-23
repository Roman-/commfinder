#include "cube_moves.h"
#include "helpers.h"
#include "easylogging++.h"

MovesArray emptyMovesArray() {
    MovesArray e;
    std::fill(e.begin(), e.end(), kNoMove);
    return e;
}

std::string moveToString(uint8_t moveIndex) {
    if (moveIndex == kNoMove || moveIndex >= kNumAllQtmMoves) {
        LOG(ERROR) << "tried to convert move #" << int(moveIndex) << " to string";
        return "?";
    }
    uint8_t prime = moveIndex / kNumQtmClockwiseMoves;
    uint8_t index = moveIndex % kNumQtmClockwiseMoves;
    return std::string(1, kCubeMovesChars[index]) + (prime?(prime == 1 ? "2" : "\'"):"");
}

bool areParallelLayersMoves(uint8_t m1, uint8_t m2) {
    constexpr std::string_view layers("memessmemessmes");
    return layers[m1 % kNumQtmClockwiseMoves] == layers[m2 % kNumQtmClockwiseMoves];
}

std::string toString(const MovesArray &moves) {
    std::string result;
    for (size_t i = 0; i < moves.size() && moves[i] != kNoMove; ++i)
        result += (i?" ":"") + moveToString(moves[i]);
    return result;
}

MovesArray stringToMoves(const std::string& scramble) {
    // TODO replace apostrophesChars =  "ʼ᾿՚’`";
    StringVec strMoves = splitString(scramble, ' ');
    MovesArray scrambleInt = emptyMovesArray();
    LOG_IF(strMoves.size() > scrambleInt.size(), FATAL) << "required to convert " << scramble
                    << " to MovesArray but max size for MovesArray is " << kMaxScrambleLength;
    for (size_t i = 0; i < strMoves.size(); ++i) {
        scrambleInt[i] = stringToMove(strMoves[i]);
    }
    return scrambleInt;
}

uint8_t stringToMove(const std::string &str) {
    if (str.size() < 1 || str.size() > 2)
        return kNoMove;
    const char mChar = str[0];
    size_t moveIndex = kCubeMovesChars.find(mChar);
    if (std::string::npos == moveIndex)
        return kNoMove;
    uint8_t result = uint8_t(moveIndex);
    if (str.size() == 1)
        return result;
    if ('2' == str[1])
        return result + kNumQtmClockwiseMoves;
    if ('\'' == str[1] || '`' == str[1])
        return result + 2*kNumQtmClockwiseMoves;
    return kNoMove;
}
