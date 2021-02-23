#include "cubestate.h"
#include "easylogging++.h"
#include "helpers.h"
#include <algorithm>

/*
  Here are the configuration/indeces of the 5x5 cube pieces in corresponding arrays:
corners, x-centers = {
    "FUL", "LFU", "ULF", // 0        0  1  2
    "LUB", "BLU", "UBL", // 1        3  4  5
    "BUR", "RBU", "URB", // 2        6  7  8
    "RUF", "FRU", "UFR", // 3        9  10 11
    "FLD", "DFL", "LDF", // 4        12 13 14
    "LBD", "DLB", "BDL", // 5        15 16 17
    "BRD", "DBR", "RDB", // 6        18 19 20
    "RFD", "DRF", "FDR"  // 7        21 22 23
};

edges, t-centers, wings (UFl-based) =  {
    "UF", "FU", // 0         0  1
    "UL", "LU", // 1         2  3
    "UR", "RU", // 2         4  5
    "UB", "BU", // 3         6  7
    "DF", "FD", // 4         8  9
    "DL", "LD", // 5         10 11
    "DR", "RD", // 6         12 13
    "DB", "BD", // 7         14 15
    "FL", "LF", // 8         16 17
    "FR", "RF", // 9         18 19
    "BL", "LB", // 10        20 21
    "BR", "RB", // 11        22 23
}; */


static StringVec cornersConfig = { "FUL", "LFU", "ULF", "LUB", "BLU", "UBL",
    "BUR", "RBU", "URB", "RUF", "FRU", "UFR", "FLD", "DFL", "LDF",
    "LBD", "DLB", "BDL", "BRD", "DBR", "RDB", "RFD", "DRF", "FDR"};

static StringVec edgesConfig = { "UF", "FU", "UL", "LU", "UR", "RU", "UB", "BU",
    "DF", "FD", "DL", "LD", "DR", "RD", "DB", "BD", "FL", "LF", "FR", "RF", "BL", "LB",
    "BR", "RB"};

static StringVec xCentersConfig = { "Ful", "Lfu", "Ulf", "Lub", "Blu", "Ubl",
    "Bur", "Rbu", "Urb", "Ruf", "Fru", "Ufr", "Fld", "Dfl", "Ldf",
    "Lbd", "Dlb", "Bdl", "Brd", "Dbr", "Rdb", "Rfd", "Drf", "Fdr"};

static StringVec tCentersConfig = { "Uf", "Fu", "Ul", "Lu", "Ur", "Ru", "Ub", "Bu",
    "Df", "Fd", "Dl", "Ld", "Dr", "Rd", "Db", "Bd", "Fl", "Lf", "Fr", "Rf", "Bl", "Lb",
    "Br", "Rb"};

static const StringVec wingsConfig = { "UFl", "FUr", "ULb", "LUf", "URf", "RUb",
    "UBr", "BUl", "DFr", "FDl", "DLf", "LDb", "DRb", "RDf", "DBl", "BDr", "FLu", "LFd",
     "FRd", "RFu", "BLd", "LBu", "BRu", "RBd"};

static const StringVec capsConfig = {"U", "F", "R", "L", "D", "B"};

StickersArray CubeState::cornersStateInitial =  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
StickersArray CubeState::edgesStateInitial =    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
StickersArray CubeState::xCentersStateInitial = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
StickersArray CubeState::tCentersStateInitial = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
StickersArray CubeState::wingsStateInitial =    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
CapsArray CubeState::capsStateInitial =         {0,1,2,3,4,5};

bool areXcenterSafe(const StickersArray& xCentersCfg) {
    constexpr std::string_view xCentersSides = "flulbubrurfufdlldbbdrrdf";
    for (uint8_t i = 0; i < xCentersCfg.size(); ++i) {
        if (xCentersSides[i] != xCentersSides[xCentersCfg[i]])
            return false;
    }
    return true;
}

bool areTcenterSafe(const StickersArray& tCentersCfg) {
    constexpr std::string_view tCentersSides = "ufulurubdfdldrdbflfrblbr";
    for (uint8_t i = 0; i < tCentersCfg.size(); ++i) {
        if (tCentersSides[i] != tCentersSides[tCentersCfg[i]])
            return false;
    }
    return true;
}

uint8_t edgeStickerNextIndex(uint8_t index) {
    return (++index % 2 == 0) ? index - 2 : index;
}

void CubeState::flipEgde(uint8_t edgeNumber) {
    std::swap(edgesState_[edgeNumber * 2], edgesState_[edgeNumber * 2 + 1]);
}

void CubeState::swapEdges(uint8_t i1, uint8_t i2) {
    if (i1 == i2)
        return;
    if (i1 / 2 == i2 / 2) // twist
        return flipEgde(i1/2);
    for (int i = 0; i < 2; ++i) {
        std::swap(edgesState_[i1], edgesState_[i2]);
        i1 = edgeStickerNextIndex(i1);
        i2 = edgeStickerNextIndex(i2);
    }
}

void CubeState::twistCorner(uint8_t cornerNumber, bool clockwise) {
    std::rotate(cornersState_.begin() + cornerNumber * 3,
                clockwise ? (cornersState_.begin() + cornerNumber * 3 + 2) :
                            (cornersState_.begin() + cornerNumber * 3 + 1),
                cornersState_.begin() + (cornerNumber+1) * 3);
}

int cornerStickerNextIndex(int index) {
    return (++index % 3 == 0) ? index - 3 : index;
}

int centerStickerIndex(const StringVec& config, const std::string& sticker) {
    int iFirstTry = indexOf(config, sticker);
    if (iFirstTry > -1)
        return iFirstTry;
    std::string stickerSpelledDifferently{sticker[0], sticker[2], sticker[1]};
    int iSecondTry = indexOf(config, stickerSpelledDifferently);
    if (iSecondTry > -1)
        return iSecondTry;
    LOG(FATAL) << "unknown center sticker: " << sticker << " nor " << stickerSpelledDifferently;
    return -1;
}

// i1, i2 - indeces of corner stickes
void CubeState::swapCorners(uint8_t i1, uint8_t i2) {
    if (i1 == i2)
        return;
    if (i1 / 3 == i2 / 3) // twist
        return twistCorner(i1/3, (i1 < i2));
    for (int i = 0; i < 3; ++i) {
        std::swap(cornersState_[i1], cornersState_[i2]);
        i1 = cornerStickerNextIndex(i1);
        i2 = cornerStickerNextIndex(i2);
    }
}

void CubeState::performCornersCycle(const std::vector<uint8_t>& cycle, uint8_t prime) {
    if (cycle.empty())
        return;
    if (prime == 1) { // double
        swapCorners(cycle[0], cycle[2]);
        swapCorners(cycle[1], cycle[3]);
    } else if (prime == 0) { // qtm
        swapCorners(cycle[3], cycle[2]);
        swapCorners(cycle[2], cycle[1]);
        swapCorners(cycle[1], cycle[0]);
    } else if (prime == 2) { // qtm prime
        swapCorners(cycle[0], cycle[1]);
        swapCorners(cycle[1], cycle[2]);
        swapCorners(cycle[2], cycle[3]);
    }
}

/// \param cycle example: Ul-Fu-Df-Bd
template <std::size_t SIZE>
void performCentersCycle(std::array<uint8_t, SIZE>& state,
                                    const std::vector<uint8_t>& cycle, uint8_t prime) {
    if (cycle.empty())
        return;
    if (prime == 1) { // double
        std::swap(state[cycle[0]], state[cycle[2]]);
        std::swap(state[cycle[1]], state[cycle[3]]);
    } else if (prime == 0) { // qtm
        std::swap(state[cycle[3]], state[cycle[2]]);
        std::swap(state[cycle[2]], state[cycle[1]]);
        std::swap(state[cycle[1]], state[cycle[0]]);
    } else if (prime == 2) { // qtm prime
        std::swap(state[cycle[0]], state[cycle[1]]);
        std::swap(state[cycle[1]], state[cycle[2]]);
        std::swap(state[cycle[2]], state[cycle[3]]);
    }
}

void CubeState::performEdgesCycle(const std::vector<uint8_t>& cycle, uint8_t prime) {
    if (cycle.empty())
        return;
    if (prime == 1) { // double
        swapEdges(cycle[0], cycle[2]);
        swapEdges(cycle[1], cycle[3]);
    } else if (prime == 0) { // qtm
        swapEdges(cycle[3], cycle[2]);
        swapEdges(cycle[2], cycle[1]);
        swapEdges(cycle[1], cycle[0]);
    } else if (prime == 2) { // qtm prime
        swapEdges(cycle[0], cycle[1]);
        swapEdges(cycle[1], cycle[2]);
        swapEdges(cycle[2], cycle[3]);
    }
}

CaseType CubeState::getCaseType(const SearchCriteria &criteria) const {
    // if caps aren't in place, that's not interesting
    if (!std::is_sorted(capsState_.begin(), capsState_.end()))
        return CaseType::caseTypeEnd;
    uint8_t mw = numMismatches(wingsState_, wingsStateInitial);
    if (mw > 5)
        return CaseType::caseTypeEnd; // saves a few computations right off
    uint8_t me = numMismatches(edgesState_, edgesStateInitial);
    uint8_t mc = numMismatches(cornersState_, cornersStateInitial);
    uint8_t mx = numMismatches(xCentersState_, xCentersStateInitial);
    uint8_t mt = numMismatches(tCentersState_, tCentersStateInitial);
    bool eSolved = (0 == me);
    bool cSolved = (0 == mc);
    bool xSolved = (0 == mx);
    bool tSolved = (0 == mt);
    bool wSolved = (0 == mw);

    // check x,t centers first - do not check centerSafetyComplied
    if (criteria.get(CaseType::x3cycles)
            && tSolved && eSolved && cSolved && wSolved
            && mx == 3)
        return CaseType::x3cycles;

    if (criteria.get(CaseType::x22swaps)
            && tSolved && eSolved && cSolved && wSolved
            && mx == 4)
        return CaseType::x22swaps;

    if (criteria.get(CaseType::x5cycles)
            && tSolved && eSolved && cSolved && wSolved
            && mx == 5)
        return CaseType::x5cycles;

    // t-centers
    if (criteria.get(CaseType::t3cycles)
            && xSolved && eSolved && cSolved && wSolved
            && mt == 3)
        return CaseType::t3cycles;

    if (criteria.get(CaseType::t22swaps)
            && xSolved && eSolved && cSolved && wSolved
            && mt == 4)
        return CaseType::t22swaps;

    if (criteria.get(CaseType::t5cycles)
            && xSolved && eSolved && cSolved && wSolved
            && mt == 5)
        return CaseType::t5cycles;

    // NOT searching for centers cases => check center safety criteria
    bool centerSafetyComplied = (criteria.getCenterSafety() == CenterSafety::StrictCenterSafe)
            ? (xSolved && tSolved) // centers strictly solved
            : (criteria.getCenterSafety() == CenterSafety::SolvedCenterSafe)
                ? centersAreSafe() // solved centers are safe
                : true;            // centers may be messed

    if (!centerSafetyComplied) {
        return CaseType::caseTypeEnd;
    } else if (criteria.get(CaseType::allSolved) && eSolved && cSolved && wSolved) {
        return CaseType::allSolved;
    }

    // then we can omit checking centers at all

    // wings
    if (criteria.get(CaseType::w2cycles)
            && eSolved && cSolved
            && mw == 2)
        return CaseType::w2cycles;

    if (criteria.get(CaseType::w3cycles)
            && eSolved && cSolved
            && mw == 3)
        return CaseType::w3cycles;

    if (criteria.get(CaseType::w22swaps)
            && eSolved && cSolved
            && mw == 4)
        return CaseType::w22swaps;

    if (criteria.get(CaseType::w5cycles)
            && eSolved && cSolved
            && mw == 5)
        return CaseType::w5cycles;

    bool hasFlips = hasEdgeFlips();
    bool hasTwists = hasCornerTwists();
    bool flippedAndSolved = edgesFlippedAndSolved();
    bool twistedAndSolved = cornersTwistedAndSolved();
    if (hasFlips && hasTwists)
        return CaseType::caseTypeEnd;

    if (criteria.get(CaseType::c3cycles)
            && eSolved && wSolved && !hasTwists
            && mc == 3*3)
        return CaseType::c3cycles;

    if (criteria.get(CaseType::c22swaps)
            && eSolved && wSolved && !hasTwists
            && mc == 4*3)
        return CaseType::c22swaps;

    if (criteria.get(CaseType::c5cycles)
            && eSolved && wSolved && !hasTwists
            && mc == 5*3)
        return CaseType::c5cycles;

    if (criteria.get(CaseType::corner2Twists)
            && eSolved && wSolved && twistedAndSolved
            && mc == 2*3)
        return CaseType::corner2Twists;

    if (criteria.get(CaseType::corner3Twists)
            && eSolved && wSolved && twistedAndSolved
            && mc == 3*3)
        return CaseType::corner3Twists;

    if (criteria.get(CaseType::corner4Twists)
            && eSolved && wSolved && twistedAndSolved
            && mc == 4*3)
        return CaseType::corner4Twists;

    // edges
    if (criteria.get(CaseType::e3cycles)
            && cSolved && wSolved && !hasFlips
            && me == 3*2)
        return CaseType::e3cycles;

    if (criteria.get(CaseType::e22swaps)
            && cSolved && wSolved && !hasFlips
            && me == 4*2)
        return CaseType::e22swaps;

    if (criteria.get(CaseType::e5cycles)
            && cSolved && wSolved && !hasFlips
            && me == 5*2)
        return CaseType::e5cycles;

    if (criteria.get(CaseType::edges2flips)
            && cSolved && wSolved && flippedAndSolved
            && me == 2*2)
        return CaseType::edges2flips;

    if (criteria.get(CaseType::edges4flips)
            && cSolved && wSolved && flippedAndSolved
            && me == 4*2)
        return CaseType::edges4flips;

    return CaseType::caseTypeEnd;
}

CubeState& CubeState::applyStringScramble(std::string scramble) {
    StringVec strMoves = splitString(scramble, ' ');
    for (const auto& moveAsString: strMoves)
        applyScrambleMove(stringToMove(moveAsString));
    return *this;
}

CubeState& CubeState::applyScramble(const MovesArray& moves) {
    for (uint8_t i = 0; i < moves.size() && moves[i] != kNoMove; ++i)
        applyScrambleMove(moves[i]);
    return *this;
}

void CubeState::applyScrambleMove(uint8_t move) {
    LOG_IF(kNoMove == move, FATAL) << "trying to apply scramble move -1";
    uint8_t prime = move / kNumQtmClockwiseMoves; // 0: qtm; 1: double; 2: prime
    // baseMove
    uint8_t baseMove = move % kNumQtmClockwiseMoves;
    const std::vector<std::vector<uint8_t>>& vec = scramblePermutations[baseMove];
    performCornersCycle(vec[0], prime); // corners
    performEdgesCycle(vec[1], prime); // edges
    performCentersCycle(xCentersState_, vec[2], prime); // x
    performCentersCycle(xCentersState_, vec[3], prime); // x
    performCentersCycle(tCentersState_, vec[4], prime); // t
    performCentersCycle(tCentersState_, vec[5], prime); // t
    performCentersCycle(wingsState_, vec[6], prime); // w
    performCentersCycle(wingsState_, vec[7], prime); // w
    performCentersCycle(capsState_, vec[8], prime); // caps

    // effect on centers being safe
    if (!isOuterMove(baseMove)) {
        // inner move destroys centers, but if they were destroyed, then Idk
        centersAreSafe_ = (Yes == centersAreSafe_) ? No : Idk;
    }
}

bool CubeState::centersAreSafe() const {
    if (Idk == centersAreSafe_)
        reCalculateIfCentersAreSafe();
    return static_cast<bool>(centersAreSafe_);
}

bool CubeState::centersAreSolved() const {
    if (No == centersAreSafe_)
        return false;
    return std::is_sorted(xCentersState_.begin(), xCentersState_.end())
        && std::is_sorted(tCentersState_.begin(), tCentersState_.end());
}

bool CubeState::cornersTwistedAndSolved() const {
    return areGroupedByButNotSorted(3, cornersState_);
}

bool CubeState::hasCornerTwists() const {
    return hasGroupedByButNotSorted(3, cornersState_);
}

bool CubeState::edgesFlippedAndSolved() const {
    return areGroupedByButNotSorted(2, edgesState_);
}

bool CubeState::hasEdgeFlips() const {
    return hasGroupedByButNotSorted(2, edgesState_);
}

CubeState& CubeState::reset() {
    cornersState_ = cornersStateInitial;
    edgesState_ = edgesStateInitial;
    wingsState_ = wingsStateInitial;
    return resetCenters();
}

CubeState &CubeState::resetCenters() {
    centersAreSafe_ = Yes;
    xCentersState_ = xCentersStateInitial;
    tCentersState_ = tCentersStateInitial;
    capsState_ = capsStateInitial;
    return *this;
}

bool CubeState::isSolved() const {
    return std::is_sorted(cornersState_.begin(), cornersState_.end())
        && std::is_sorted(edgesState_.begin(), edgesState_.end())
        && std::is_sorted(wingsState_.begin(), wingsState_.end())
        && centersAreSolved();
}

// WARNING this doesn't work for corners and edges (elements with 2+ stickers on them)
template<size_t SIZE>
std::string solveAndGetElemCycles(std::array<uint8_t, SIZE>& state
                                  , const StringVec& config ) {
    std::string desc;
    // while (numMismatches(state, stateInitial) > 0) {
    while (!std::is_sorted(state.begin(), state.end())) {
        uint8_t i = 0;
        for (;state[i] == i;++i) {} // t[i] != i
        while (state[i] != i) {
            desc += config[state[i]] + "-";
            std::swap(state[i], state[state[i]]);
        }
        desc += config[state[i]] + ".";
    }
    return desc;
}

// param corner - if true, do this for corners, else do for edges
std::string CubeState::solveAndGetMultistickerCycles(std::array<uint8_t, kNumElemStickers>& state
                                  , const StringVec& config, bool corner) {
    std::string desc;
    while (!std::is_sorted(state.begin(), state.end())) {
        uint8_t i = 0;
        for (;state[i] == i;++i) {} // t[i] != i
        desc += config[i];
        while (state[i] != i) {
            desc += "-" + config[state[i]];
            if (corner)
                swapCorners(i, state[i]);
            else
                swapEdges(i, state[i]);
        }
        desc += ".";
    }
    return desc;
}

std::string CubeState::solveAndGetCycles(bool ignoreCentersIfCenterSafe) {
    if (ignoreCentersIfCenterSafe && centersAreSafe()) {
        // restore centers as if they were solved already
        tCentersState_ = tCentersStateInitial;
        xCentersState_ = xCentersStateInitial;
    }
    centersAreSafe_ = Yes;
    return
        solveAndGetMultistickerCycles(cornersState_, cornersConfig, true) +
        solveAndGetMultistickerCycles(edgesState_, edgesConfig, false) +
        solveAndGetElemCycles(tCentersState_, tCentersConfig) +
        solveAndGetElemCycles(xCentersState_, xCentersConfig) +
        solveAndGetElemCycles(wingsState_, wingsConfig) +
        solveAndGetElemCycles(capsState_, capsConfig);
}

std::string CubeState::whichElementsAreUnsolved() const {
    std::string r;
    for (uint8_t i = 0; i < xCentersStateInitial.size(); ++i)
        if (xCentersState_[i] != xCentersStateInitial[i])
            r += xCentersConfig[i] + " ";
    for (uint8_t i = 0; i < tCentersStateInitial.size(); ++i)
        if (tCentersState_[i] != tCentersStateInitial[i])
            r += tCentersConfig[i] + " ";
    for (uint8_t i = 0; i < wingsStateInitial.size(); ++i)
        if (wingsState_[i] != wingsStateInitial[i])
            r += wingsConfig[i] + " ";

    return r;
}

std::string CubeState::toString() const {
    std::string s = "CubeState: {\n";
    s += "c: " + arrToString(cornersState_)
            + (cornersState_ == cornersStateInitial ? " // solved\n":"\n");
    s += "e: " + arrToString(edgesState_)
            + (edgesState_ == edgesStateInitial ? " // solved\n":"\n");
    s += "w: " + arrToString(wingsState_)
            + (wingsState_ == wingsStateInitial ? " // solved\n":"\n");
    s += "x: " + arrToString(xCentersState_)
            + (xCentersState_ == xCentersStateInitial ? " // solved\n":"\n");
    s += "t: " + arrToString(tCentersState_)
            + (tCentersState_ == tCentersStateInitial ? " // solved\n":"\n");
    s += "^: " + arrToString(capsState_)
            + (capsState_ == capsStateInitial ? " // solved\n":"\n");
    s += "}";
    return s;
}

std::ostream& operator<<(std::ostream& oss, const CubeState& cube) {
    return oss << cube.toString();
}

uint16_t CubeState::totalNumMismatches() const {
    return
        numMismatches(cornersState_, cornersStateInitial) +
        numMismatches(capsState_, capsStateInitial) +
        numMismatches(xCentersState_, xCentersStateInitial) +
        numMismatches(tCentersState_, tCentersStateInitial) +
        numMismatches(edgesState_, edgesStateInitial) +
            numMismatches(wingsState_, wingsStateInitial);
}

void CubeState::reCalculateIfCentersAreSafe() const {
    bool safe = std::is_sorted(capsState_.begin(), capsState_.end())
                && areXcenterSafe(xCentersState_)
                && areTcenterSafe(tCentersState_);
    centersAreSafe_ = safe ? Yes : No;
}
