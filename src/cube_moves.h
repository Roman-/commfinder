#ifndef CUBE_MOVES_H
#define CUBE_MOVES_H
#include <string>
#include <vector>
#include <array>

// uint8_t move indeces correspond to moves in this string
constexpr std::string_view kCubeMovesChars = "LURDFBlurdfbMES";

// special value denoting invalid move / end of move sequence
constexpr uint8_t kNoMove = uint8_t(-1);

// number of possible moves qtm moves = number of "layers" = 15
constexpr uint8_t kNumQtmClockwiseMoves = kCubeMovesChars.size();

inline uint8_t baseMove(uint8_t m) {return m % kNumQtmClockwiseMoves;}

// number of all possible 5x5 HTM moves = (6+6+3)*3 = 45.
// foreach move X, count (X,X2,X')
constexpr uint8_t kNumAllQtmMoves = kCubeMovesChars.size() * 3;

// number of sides on any cube also corresponds to number of colors
// and number of caps ("center centers") - square pieces
constexpr uint8_t kNumSides = 6;
constexpr uint8_t kNumCorners = 8;
constexpr uint8_t kNumEdges = 12;

// total number of stickers for edges, corners, x-centers, t-centers and wings*
constexpr uint8_t kNumElemStickers = 4 * kNumSides;

using StickersArray = std::array<uint8_t, kNumElemStickers>;
using CapsArray = std::array<uint8_t, kNumSides>;

// max.scramble length used in: iterative scramble; bruteforce solver
constexpr uint8_t kMaxScrambleLength = 10;
using MovesArray = std::array<uint8_t, kMaxScrambleLength>;

// returns {-1,-1,-1...}
MovesArray emptyMovesArray();

using StringVec = std::vector<std::string>;

// returns true if e.g.
inline bool areMovesOfSameFace(uint8_t m1, uint8_t m2) {
    return (kNoMove == m1 || kNoMove == m2 || m1 >= kNumAllQtmMoves || m2 >= kNumAllQtmMoves)
            ? false
            : (m1 % kNumQtmClockwiseMoves == m2 % kNumQtmClockwiseMoves);
}

// returns id of move opposite to @param move. Double move is opposite to itself (L2=L2')
inline uint8_t oppoMove(uint8_t move) {
    constexpr uint8_t kDMRange = kNumQtmClockwiseMoves * 2; //
    return ((move >= kNumQtmClockwiseMoves && move < kDMRange) // double?
            ? move // double.
            : ((move < kNumQtmClockwiseMoves)
                   ? move+kDMRange    // R  -> R'
                   : move-kDMRange ));// R' -> R
}

// returns true if moves m1,m2 are on parallel (or same) layer.
// examples: [u', D], [S, F2] are parallel; [u, f] are not
bool areParallelLayersMoves(uint8_t m1, uint8_t m2);

// returns true if m is outer layer move (LURDFB)
inline bool isOuterMove(uint8_t m) {
    // see if baseMove is on first 6 positions in string "LURDFBlurdfbMES"
    return (m % kNumQtmClockwiseMoves < 6);
}

// string <-> moves conversions

// convert uint8 move to human-readable string
std::string moveToString(uint8_t moveIndex);

// convert uint8 move to human-readable string
std::string toString(const MovesArray& moves);

// convert string to single move. Returns kNoMove if invalid
uint8_t stringToMove(const std::string& str);

// convert human-readable scramble to movesArray
// should follow strict syntax: one space between moves, QTM + primes only, no wide moves etc.
MovesArray stringToMoves(const std::string& scramble);

// scramblePermutations: line #i corresponds to permutations move#i does
// Order of elements: {c, e, x1, x2, t1, t2, w1, w2}
// e.g. line#0: {{1,14,15,3}, ....} means move #0 (which is L) permutes 4 corners clockwise: 1->14->15->3->1
const std::vector<std::vector<std::vector<uint8_t>>> scramblePermutations = {
//   corners         edges        x-centers 1  x2  t-centers     t2   wings1       wings2      caps
    {{1,14,15,3},  {3,17,11,21}, {1,14,15,3},  {}, {3,17,11,21}, {}, {3,17,11,21},{2,16,10,20}, {}}, // L
    {{5,8,11,2},   {0,2,6,4},    {5,8,11,2},   {}, {0,2,6,4},    {}, {0,2,6,4},   {1,3,7,5},    {}}, // U
    {{7,20,21,9},  {19,5,23,13}, {7,20,21,9},  {}, {19,5,23,13}, {}, {19,5,23,13},{18,4,22,12}, {}}, // R
    {{13,22,19,16},{8,12,14,10}, {13,22,19,16},{}, {8,12,14,10}, {}, {8,12,14,10},{9,13,15,11}, {}}, // D
    {{0,10,23,12}, {1,18,9,16},  {0,10,23,12}, {}, {1,18,9,16},  {}, {1,18,9,16}, {0,19,8,17},  {}}, // F
    {{6,4,17,18},  {7,20,15,22}, {6,4,17,18},  {}, {7,20,15,22}, {}, {7,20,15,22},{6,21,14,23}, {}}, // B
//   c,   e,   x-centers-1  x-centers-2    t-centers    t2,   wings1,     w2 caps
    {{},  {}, {5,0,13,17},  {2,12,16,4},  {2,16,10,20}, {}, {0,9,14,7},   {}, {}}, // l
    {{},  {}, {10,1,4,7},   {0,3,6,9},    {1,3,7,5},    {}, {16,21,22,19},{}, {}}, // u
    {{},  {}, {8,18,22,10}, {11,6,19,23}, {4,22,12,18}, {}, {6,15,8,1},   {}, {}}, // r
    {{},  {}, {12,21,18,15},{23,20,17,14},{9,13,15,11}, {}, {18,23,20,17},{}, {}}, // d
    {{},  {}, {2,9,22,14},  {11,21,13,1}, {0,19,8,17},  {}, {4,13,10,3},  {}, {}}, // f
    {{},  {}, {8,3,16,20},  {5,15,19,7},  {6,21,14,23}, {}, {2,11,12,5},  {}, {}}, // b
//   c,   edges,       x1  x2   t-centers-1  t-centers-2,  w1, w2,  centers
    {{}, {0,9,14,7},   {}, {}, {0,9,14,7},   {6,1,8,15},   {}, {}, {0,1,4,5}}, // M
    {{}, {18,23,20,17},{}, {}, {18,23,20,17},{16,19,22,21},{}, {}, {1,2,5,3}}, // E
    {{}, {4,13,10,3},  {}, {}, {4,13,10,3},  {2,5,12,11},  {}, {}, {0,2,4,3}}  // S
};



#endif // CUBE_MOVES_H
