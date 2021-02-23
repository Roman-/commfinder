#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_set>

#include <bruteforcesolver.h>
#include <cubestate.h>
#include <cube_moves.h>
#include <helpers.h>
#include <incrementalscramble.h>
#include <searchcriteria.h>
#include <commutatorfinder.h>

#include "testalgs.h"

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP

using namespace std::chrono_literals;

////////////////////////////////////// helpers //////////////////////////////
TEST(Helpers, AreGroupedBy) {
    std::array<uint8_t, 8> a1 = {0,1,2,3,4,5,6,7};
    ASSERT_TRUE(areGroupedBy(4, a1));

    std::array<uint8_t, 8> a2 = {0,3,2,3,4,5,6,7};
    ASSERT_TRUE(areGroupedBy(4, a2));

    std::array<uint8_t, 8> a3 = {3,2,1,0,6,5,5,5};
    ASSERT_TRUE(areGroupedBy(4, a3));

    std::array<uint8_t, 8> b1 = {0,1,2,4,4,5,6,7};
    ASSERT_FALSE(areGroupedBy(4, b1));

    // all arrays are grouped by their size
    std::array<uint8_t, 8> c1 = {5,4,2,1,7,6,2,1};
    ASSERT_TRUE(areGroupedBy(c1.size(), c1));
}

TEST(Helpers, StringOperations) {
    StringVec helloWorldVector{"hello", "world"};
    std::string helloWorldString("hello world");
    ASSERT_EQ(splitString(helloWorldString, ' '), helloWorldVector);
    ASSERT_STREQ(joinStrings(helloWorldVector, ' ').c_str(), helloWorldString.c_str());

    ASSERT_NE(splitString(helloWorldString + "\t", ' '), helloWorldVector);
}

TEST(Helpers, GroupedByButNotSorted) {
    std::array<uint8_t, 8> a1 = {0,1,2,3,4,5,6,7};
    ASSERT_FALSE(areGroupedByButNotSorted(4, a1));

    std::array<uint8_t, 8> a2 = {0,3,2,3,4,5,6,7};
    ASSERT_TRUE(areGroupedByButNotSorted(4, a2));

    std::array<uint8_t, 8> a3 = {3,2,1,0,6,5,5,5};
    ASSERT_TRUE(areGroupedByButNotSorted(4, a3));

    std::array<uint8_t, 8> b1 = {0,1,2,4,4,5,6,7};
    ASSERT_FALSE(areGroupedByButNotSorted(4, b1));

    // all arrays are grouped by their size
    std::array<uint8_t, 8> c1 = {5,4,2,1,7,6,2,1};
    ASSERT_TRUE(areGroupedByButNotSorted(c1.size(), c1));
}

TEST(Helpers, hasGroupedByButNotSorted) {
    std::array<uint8_t, 8> a1 = {0,1,2,3,4,5,6,7};
    ASSERT_FALSE(hasGroupedByButNotSorted(4, a1)); // because sorted

    std::array<uint8_t, 8> a2 = {1,2,0,3,4,5,6,7};
    ASSERT_FALSE(hasGroupedByButNotSorted(4, a2)); // because 3 is in its position

    std::array<uint8_t, 8> a3 = {0,2,1,3,4,5,6,7};
    ASSERT_FALSE(hasGroupedByButNotSorted(4, a3)); // because 2 and 3 are in their positions

    std::array<uint8_t, 8> b1 = {1,2,3,0,4,5,6,7};
    ASSERT_TRUE(hasGroupedByButNotSorted(4, b1));

    std::array<uint8_t, 9> b2 = {0,1,2,4,5,3,6,7,8};
    ASSERT_TRUE(hasGroupedByButNotSorted(3, b2));

    std::array<uint8_t, 9> c1 = {0,1,2,3,5,4,6,7,8};
    ASSERT_FALSE(hasGroupedByButNotSorted(3, c1));
}

////////////////////////////////////// cube_moves //////////////////////////////////////
TEST(CubeMoves, Constants) {
    ASSERT_EQ(kNumElemStickers, 24);
    ASSERT_EQ(kNumSides, 6);
    EXPECT_GE(kMaxScrambleLength, 5) << "are you sure it\'s a good idea?";
    ASSERT_GE(emptyMovesArray().size(), kMaxScrambleLength);
    ASSERT_EQ(emptyMovesArray().front(), kNoMove);
}

TEST(CubeMoves, BaseMove) {
    std::string faces(kCubeMovesChars);
    for (const char faceChar: faces) {
        auto fStr = std::string(1,faceChar);
        auto fpStr = fStr + "\'";
        auto fppStr = fStr + "`";
        auto f2Str = fStr + "2";

        uint8_t f = stringToMove(fStr);
        uint8_t fp = stringToMove(fpStr);
        uint8_t fpp = stringToMove(fppStr);
        uint8_t f2 = stringToMove(f2Str);

        uint8_t fBase = baseMove(f);
        uint8_t fpBase = baseMove(fp);
        uint8_t fppBase = baseMove(fpp);
        uint8_t f2Base= baseMove(f2);

        ASSERT_EQ(fBase, f) << int(fBase) << "," << int(f) << " = " << fStr;
        ASSERT_EQ(fpBase, f) << fpStr;
        ASSERT_EQ(fppBase, f) << fppStr;
        ASSERT_EQ(f2Base, f) << fStr;
    }
}

TEST(CubeMoves, isOuterMove) {
    ASSERT_TRUE(isOuterMove(stringToMove("R")));
    ASSERT_TRUE(isOuterMove(stringToMove("D2")));
    ASSERT_TRUE(isOuterMove(stringToMove("L\'")));

    ASSERT_FALSE(isOuterMove(stringToMove("M\'")));
    ASSERT_FALSE(isOuterMove(stringToMove("S2")));
    ASSERT_FALSE(isOuterMove(stringToMove("l")));
    ASSERT_FALSE(isOuterMove(stringToMove("f\'")));
    ASSERT_FALSE(isOuterMove(stringToMove("b\'")));
}

TEST(CubeMoves, MovesStringsSingles) {
    std::string faces(kCubeMovesChars);
    for (const auto& f: faces) {
        auto fMove = std::string(1,f);
        auto fPrime1 = fMove + "\'";
        auto fPrime2 = fMove + "`";
        auto f2 = fMove + "2";
        ASSERT_NE(stringToMove(fMove), kNoMove) << fMove;
        ASSERT_NE(stringToMove(fPrime1), kNoMove) << fPrime1;
        ASSERT_NE(stringToMove(fPrime2), kNoMove) << fPrime2;
        ASSERT_NE(stringToMove(f2), kNoMove) << f2;

        ASSERT_EQ(stringToMove(fMove + fMove), kNoMove) << fMove;
    }

    StringVec invalidMoves{"", " ", "R\'\'", " \'", "m", "s", "e"};
    for (const auto & iv: invalidMoves)
        ASSERT_EQ(stringToMove(iv), kNoMove) << iv;
}

TEST(CubeMoves, MovesStrings) {
    // LURDFBlurdfbMES
    EXPECT_STREQ(moveToString(0).c_str(), "L");

    auto scr = emptyMovesArray();
    for (int i = 0; i < 4; ++i)
        scr[i] = uint8_t(i);
    EXPECT_STREQ(toString(scr).c_str(), "L U R D");
}



TEST(CubeMoves, SameFace) {
    std::string faces(kCubeMovesChars);
    constexpr uint8_t moveOutOfBounds = uint8_t(kNumAllQtmMoves);
    for (char f: faces) {
        std::string fMove(1, f);
        std::string fpMove = fMove + "\'";
        std::string f2Move = fMove + "2";
        uint8_t indexOfCw = stringToMove(fMove);
        uint8_t indexOfCcw = stringToMove(fpMove);
        uint8_t indexOf2 = stringToMove(f2Move);
        ASSERT_TRUE(areMovesOfSameFace(indexOfCw, indexOfCw));
        ASSERT_TRUE(areMovesOfSameFace(indexOfCw, indexOfCcw));
        ASSERT_TRUE(areMovesOfSameFace(indexOfCw, indexOf2));

        uint8_t indexOfNextMove = indexOfCw+1;
        ASSERT_FALSE(areMovesOfSameFace(indexOfCw, indexOfNextMove));
        ASSERT_FALSE(areMovesOfSameFace(indexOfCcw, indexOfNextMove));
        ASSERT_FALSE(areMovesOfSameFace(indexOf2, indexOfNextMove));

        ASSERT_FALSE(areMovesOfSameFace(kNoMove, indexOfCw));
        ASSERT_FALSE(areMovesOfSameFace(kNoMove, indexOfCcw));
        ASSERT_FALSE(areMovesOfSameFace(indexOfCcw, kNoMove));

        ASSERT_FALSE(areMovesOfSameFace(moveOutOfBounds, indexOfCw));
        ASSERT_FALSE(areMovesOfSameFace(moveOutOfBounds, indexOfCcw));
        ASSERT_FALSE(areMovesOfSameFace(indexOfCcw, moveOutOfBounds));
    }

    ASSERT_FALSE(areMovesOfSameFace(kNoMove, kNoMove));
    ASSERT_FALSE(areMovesOfSameFace(moveOutOfBounds, kNoMove));
    ASSERT_FALSE(areMovesOfSameFace(kNoMove, moveOutOfBounds));
    ASSERT_FALSE(areMovesOfSameFace(moveOutOfBounds, moveOutOfBounds));
}

TEST(CubeMoves, ParallelFace) {
    ASSERT_FALSE(areParallelLayersMoves(stringToMove("r"), stringToMove("f")));
    ASSERT_FALSE(areParallelLayersMoves(stringToMove("S"), stringToMove("M")));
    ASSERT_FALSE(areParallelLayersMoves(stringToMove("D"), stringToMove("M\'")));

    ASSERT_TRUE(areParallelLayersMoves(stringToMove("r"), stringToMove("r")));
    ASSERT_TRUE(areParallelLayersMoves(stringToMove("r"), stringToMove("r\'")));
    ASSERT_TRUE(areParallelLayersMoves(stringToMove("S2"), stringToMove("S\'")));
    ASSERT_TRUE(areParallelLayersMoves(stringToMove("d"), stringToMove("D\'")));
    ASSERT_TRUE(areParallelLayersMoves(stringToMove("R"), stringToMove("M\'")));
    ASSERT_TRUE(areParallelLayersMoves(stringToMove("M"), stringToMove("l2")));
    ASSERT_TRUE(areParallelLayersMoves(stringToMove("F"), stringToMove("S2")));
}

TEST(CubeMoves, OppositeMoves) {
    std::string faces(kCubeMovesChars);
    for (char f: faces) {
        std::string moveCw(1,f);
        std::string moveCcw = moveCw + "\'";
        std::string move2 = moveCw + "2";
        ASSERT_EQ(oppoMove(stringToMove(moveCw)), stringToMove(moveCcw)) << moveCw << " and " << moveCcw;
        ASSERT_EQ(oppoMove(stringToMove(moveCcw)), stringToMove(moveCw));
        ASSERT_EQ(oppoMove(stringToMove(move2)), stringToMove(move2));

        ASSERT_NE(oppoMove(stringToMove(move2)), stringToMove(moveCw));
        ASSERT_NE(oppoMove(stringToMove(move2)), stringToMove(moveCcw));
        ASSERT_NE(oppoMove(stringToMove(moveCw)), stringToMove(moveCw));
        ASSERT_NE(oppoMove(stringToMove(moveCw)), stringToMove(move2));
        ASSERT_NE(oppoMove(stringToMove(moveCcw)), stringToMove(moveCcw));
    }

    ASSERT_NE(oppoMove(stringToMove("r")), stringToMove("S"));
    ASSERT_NE(oppoMove(stringToMove("r")), stringToMove("f"));
    ASSERT_NE(oppoMove(stringToMove("r")), stringToMove("R"));
    ASSERT_NE(oppoMove(stringToMove("r")), stringToMove("R\'"));
}

////////////////////////////////////// incrementalscramble //////////////////////////////
TEST(IncrementalScramble, ItrScrGeneratesAllScrambles) {
    std::unordered_set<std::string> hash;
    for (IncrementalScramble is; is.size() <= 3; ++is) {
        auto moves = is.get();
        ASSERT_LT(is.size(), moves.size());

        // make sure all moves are valid
        for (size_t j = 0; j < is.size(); ++j) {
            ASSERT_LT(moves[j], kNumAllQtmMoves) << j;
            ASSERT_NE(moves[j], kNoMove) << j;
        }
        // next move is -1
        ASSERT_EQ(moves[is.size()], kNoMove);

        std::string scramble = toString(moves);
        // doesn't generate same scramble twice
        ASSERT_TRUE(hash.insert(scramble).second) << scramble;
    }

    // make sure random sub-3-movers are inserted
    ASSERT_TRUE(hash.cend() != hash.find("M"));
    ASSERT_TRUE(hash.cend() != hash.find("M\'"));
    ASSERT_TRUE(hash.cend() != hash.find("M2"));

    // make sure random 3-movers are inserted
    std::string allQtmMoves(kCubeMovesChars);
    // make sure it has all permutations of these moves
    for (size_t i = 0; i < allQtmMoves.size(); ++i) {
        for (size_t j = i+1; j < allQtmMoves.size(); ++j) {

            // assert it has all 2-outer-moves scrambles
            std::string twoMoveScr1 = moveToString(i) + " " + moveToString(j);
            std::string twoMoveScr2 = moveToString(j) + " " + moveToString(i);
            bool hasScr1 = hash.cend() != hash.find(twoMoveScr1);
            bool hasScr2 = hash.cend() != hash.find(twoMoveScr2);
            if (areParallelLayersMoves(i,j)) {
                ASSERT_TRUE(hasScr1 ^ hasScr2) << twoMoveScr1 << "," << twoMoveScr2
                                               << "; Has them? " << hasScr1 << hasScr2;
            } else {
                ASSERT_TRUE(hasScr1 && hasScr2) << twoMoveScr1 << "," << twoMoveScr2;
            }

            // redundant scrambles not inserted
            auto rm1 = moveToString(i);
            auto rm2 = moveToString(i%kNumQtmClockwiseMoves);
            auto rm3 = moveToString(j);
            std::vector<std::string> redundantScrambles = {
                joinStrings({rm1, rm2},' '),
                joinStrings({rm2, rm1},' '),
                joinStrings({rm1, rm2, rm3},' '),
                joinStrings({rm2, rm1, rm3},' '),
                joinStrings({rm3, rm1, rm2},' '),
                joinStrings({rm3, rm2, rm1},' '),
            };
            for (const auto& rdScr: redundantScrambles) {
                ASSERT_TRUE(hash.cend() == hash.find(rdScr)) << rdScr << " is redundant";
            }

            // don't bother checking if parallel move scrambles (U R L) or (L U R) are there
            if (areParallelLayersMoves(i, j))
                continue;
            // assert it has all 3-outer-moves scrambles
            for (size_t k = j+1; k < allQtmMoves.size(); ++k) {
                // same: don't bother checking parallel move scrambles - will be checked later
                if (areParallelLayersMoves(k, i) || areParallelLayersMoves(k, j))
                    continue;
                std::vector<size_t> indeces{i,j,k};
                while (std::next_permutation(indeces.begin(), indeces.end())) {
                    StringVec movesString = {
                        moveToString(indeces[0]),
                        moveToString(indeces[1]),
                        moveToString(indeces[2])
                    };
                    std::string scramble = joinStrings(movesString, ' ');
                    ASSERT_TRUE(hash.cend() != hash.find(scramble)) << scramble;
                    auto scramble2 = scramble + "2";
                    ASSERT_TRUE(hash.cend() != hash.find(scramble2)) << scramble2;
                    auto scramblePrime = scramble + "\'";
                    ASSERT_TRUE(hash.cend() != hash.find(scramblePrime)) << scramblePrime;
                }
            }
        }
    }

    // random manual redundant algs
    ASSERT_TRUE(hash.cend() == hash.find("M2 M"));
    ASSERT_TRUE(hash.cend() == hash.find("L L L"));
    ASSERT_TRUE(hash.cend() == hash.find("L L U"));
    ASSERT_TRUE(hash.cend() == hash.find("L U U"));
    ASSERT_TRUE(hash.cend() == hash.find("l\' M l"));

    // only insert one of "R L" and "L R"
    std::string parallelToL = "LlMrR";
    for (size_t i = 0; i < parallelToL.size(); ++i) {
        for (size_t j = i+1; j < parallelToL.size(); ++j) {
            std::string m1(1,parallelToL[i]);
            std::string m2(1,parallelToL[j]);
            bool has12 = hash.cend() != hash.find(m1 + " " + m2);
            bool has21 = hash.cend() != hash.find(m2 + " " + m1);
            // either has "L R" or "R L" but not both and not none
            ASSERT_TRUE(has12 ^ has21) << "m1,m2= " << m1 << "," << m2
                                       << "has 12,21 = " << has12 << "," << has21;
        }
    }
}

TEST(IncrementalScramble, incAndSkipParallelL) {
    IncrementalScramble isL;
    std::unordered_set<std::string> hashNoL;
    uint8_t moveL = stringToMove("L");
    while (isL.size() <= 3) {
        isL.incAndSkipParallelBeginEnd(moveL);
        // insert each scramble once
        std::string scramble = isL.toString();
        ASSERT_TRUE(hashNoL.insert(scramble).second) << scramble;
    }
    ASSERT_TRUE(hashNoL.end() == hashNoL.find("L U"));
    ASSERT_TRUE(hashNoL.end() == hashNoL.find("U L"));
    ASSERT_TRUE(hashNoL.end() == hashNoL.find("U L\'"));
    ASSERT_TRUE(hashNoL.end() == hashNoL.find("L U R"));
    ASSERT_TRUE(hashNoL.end() == hashNoL.find("R U F"));
    ASSERT_TRUE(hashNoL.end() == hashNoL.find("U l\'"));

    ASSERT_TRUE(hashNoL.end() != hashNoL.find("F U"));
    ASSERT_TRUE(hashNoL.end() != hashNoL.find("D L U"));
    ASSERT_TRUE(hashNoL.end() != hashNoL.find("D L U"));
}

TEST(IncrementalScramble, incAndSkipParallelf) {
    IncrementalScramble isf;
    std::unordered_set<std::string> hashNof;
    uint8_t movef = stringToMove("f");
    while (isf.size() <= 3) {
        isf.incAndSkipParallelBeginEnd(movef);
        // insert each scramble once
        std::string scramble = isf.toString();
        ASSERT_TRUE(hashNof.insert(scramble).second) << scramble;
    }
    ASSERT_TRUE(hashNof.end() == hashNof.find("F U"));
    ASSERT_TRUE(hashNof.end() == hashNof.find("U F"));
    ASSERT_TRUE(hashNof.end() == hashNof.find("S\'"));
    for (char mv: kCubeMovesChars) {
        ASSERT_TRUE(hashNof.end() == hashNof.find(std::string(1,mv) + " S\'")) << mv;
        ASSERT_TRUE(hashNof.end() == hashNof.find(std::string(1,mv) + " S2")) << mv;
        ASSERT_TRUE(hashNof.end() == hashNof.find(std::string(1,mv) + " S")) << mv;
    }
    ASSERT_TRUE(hashNof.end() == hashNof.find("f2"));
    ASSERT_TRUE(hashNof.end() == hashNof.find("L f2"));
    ASSERT_TRUE(hashNof.end() == hashNof.find("f2 L"));

    ASSERT_TRUE(hashNof.end() != hashNof.find("D f U"));
    ASSERT_TRUE(hashNof.end() != hashNof.find("D S U"));
    for (char mv: std::string("fFSbB")) {
        ASSERT_TRUE(hashNof.end() != hashNof.find("R " + std::string(1,mv) + " D")) << mv;
        ASSERT_TRUE(hashNof.end() != hashNof.find("R " + std::string(1,mv) + "2 D")) << mv;
        ASSERT_TRUE(hashNof.end() != hashNof.find("R " + std::string(1,mv) + "\' D")) << mv;
    }
}

////////////////////////////////////// CubeStateTests //////////////////////////////////////
TEST(CubeStateTests, SetAndResetCubeState) {
    auto cube = CubeState().applyStringScramble("R u");
    ASSERT_FALSE(cube.isSolved());
    ASSERT_FALSE(cube.centersAreSolved());
    ASSERT_FALSE(cube.centersAreSafe());
    cube.reset();
    ASSERT_TRUE(cube.isSolved());
    ASSERT_TRUE(cube.centersAreSafe());
    ASSERT_TRUE(cube.centersAreSolved());
    ASSERT_FALSE(cube.hasEdgeFlips());
}

TEST(CubeStateTests, SetAndResetCenters) {
    auto cube = CubeState().applyStringScramble("R u");
    ASSERT_FALSE(cube.centersAreSafe());
    ASSERT_FALSE(cube.centersAreSolved());
    cube.resetCenters();
    ASSERT_TRUE(cube.centersAreSafe());
    ASSERT_TRUE(cube.centersAreSolved());
    ASSERT_FALSE(cube.isSolved());
}

TEST(CubeStateTests, defaultConstructedCubeStateIsSolved) {
    CubeState defaultConstructedCubeState;
    ASSERT_TRUE(defaultConstructedCubeState.isSolved());
    ASSERT_TRUE(defaultConstructedCubeState.centersAreSafe());
    ASSERT_TRUE(defaultConstructedCubeState.centersAreSolved());

    // applying any move should make state not solved
    defaultConstructedCubeState.applyScrambleMove(0);
    ASSERT_FALSE(defaultConstructedCubeState.isSolved());
}

TEST(CubeStateTests, SixSexySolvesTheCube) {
    CubeState cube;
    for (size_t i = 0; i < 6; ++i)
        cube.applyStringScramble("R U R\' U\'");
    ASSERT_TRUE(cube.isSolved());
    ASSERT_TRUE(cube.centersAreSafe());
    ASSERT_TRUE(cube.centersAreSolved());
}

TEST(CubeStateTests, TotalNumMismatches) {
    CubeState cube;
    ASSERT_EQ(cube.totalNumMismatches(), 0);
    cube.applyStringScramble("R");
    ASSERT_GT(cube.totalNumMismatches(), 3);
    cube.applyStringScramble("R R R");
    ASSERT_EQ(cube.totalNumMismatches(), 0);
}

TEST(CubeStateTests, GettingCyclesSolvesTheCube) {
    CubeState cube;
    for (uint8_t move = 0; move < kNumAllQtmMoves; ++move) {
        cube.applyScrambleMove(move);
        std::string cycles = cube.solveAndGetCycles();
        EXPECT_TRUE(cube.isSolved()) << cycles << cube;
        EXPECT_GT(cycles.size(), 0);
    }
}
TEST(CubeStateTests, CenterSafeSimple) {
    CubeState cube;

    ASSERT_TRUE(cube.centersAreSafe()) << cube;
    cube.applyStringScramble("U l l\'");
    ASSERT_TRUE(cube.centersAreSafe()) << cube;
    ASSERT_FALSE(cube.centersAreSolved());
    cube.applyStringScramble("U");
    ASSERT_TRUE(cube.centersAreSafe()) << cube;
    ASSERT_FALSE(cube.centersAreSolved());
}

TEST(CubeStateTests, CenterSafe) {
    CubeState cube;

    cube.applyStringScramble("R U F B D");

    ASSERT_TRUE(cube.centersAreSafe()) << cube;
    ASSERT_FALSE(cube.centersAreSolved());
    ASSERT_FALSE(cube.isSolved()) << cube;
    cube.applyStringScramble("l");
    ASSERT_FALSE(cube.centersAreSafe()) << cube;
    ASSERT_FALSE(cube.centersAreSolved());
    ASSERT_FALSE(cube.isSolved()) << cube;
    cube.applyStringScramble("l\'");
    ASSERT_TRUE(cube.centersAreSafe()) << cube;
    ASSERT_FALSE(cube.isSolved()) << cube;
    cube.applyStringScramble("D\' B\' F\' U\' R\'");
    ASSERT_TRUE(cube.centersAreSafe()) << cube;
    ASSERT_TRUE(cube.isSolved()) << cube;
}

TEST(CubeStateTests, CenterSafeOnMUalgs) {
    auto cube = CubeState().applyStringScramble("M U M` U2 M U M`");
    ASSERT_TRUE(cube.centersAreSafe()) << cube;

    cube.reset().applyStringScramble("M U M` U M U M` U");
    ASSERT_FALSE(cube.centersAreSafe()) << cube;
}

TEST(CubeStateTests, TrifleSolvesCube) {
    std::string faces(kCubeMovesChars);
    for (uint8_t move = 0; move < kNumAllQtmMoves; ++move) {
        // check if applying move 4 times solves cube
        CubeState cube;
        for (size_t i = 0; i < 4; ++i)
            cube.applyScrambleMove(move);
        ASSERT_TRUE(cube.isSolved());
    }

    for (uint8_t move = 0; move < kNumAllQtmMoves; ++move) {
        // check if doing R R\' solves cube
        CubeState cube;
        cube.applyScrambleMove(move);
        cube.applyScrambleMove(oppoMove(move));
        ASSERT_TRUE(cube.isSolved());
    }

    for (uint8_t move = 0; move < kNumAllQtmMoves; ++move) {
        // check if applying any move to cube makes it unsolved
        CubeState cube;
        cube.applyScrambleMove(move);
        ASSERT_FALSE(cube.isSolved());
    }
}

////////////////////////////////////// SearchCriteria //////////////////////////////////////
TEST(SearchCriteria, SearchCriteriaIntegrity) {
    SearchCriteria scTrue(true);
    for (size_t i = 0; i < size_t(CaseType::caseTypeEnd); ++i) {
        CaseType ct = static_cast<CaseType>(i);
        ASSERT_TRUE(CaseType::allSolved == ct || scTrue.get(ct));
        scTrue.set(CaseType(i), false);
        ASSERT_FALSE(scTrue.get(ct));
    }

    SearchCriteria scFalse(false);
    for (size_t i = 0; i < size_t(CaseType::caseTypeEnd); ++i) {
        CaseType ct = static_cast<CaseType>(i);
        ASSERT_FALSE(scTrue.get(ct));
        scTrue.set(CaseType(i), true);
        ASSERT_TRUE(scTrue.get(ct));
    }
}

TEST(SearchCriteria, CubeMatchesCriteria1) {
    const std::string alg = "M U M` U2 M U M`"; // solved-center-safe
    SearchCriteria criteria(true, CenterSafety::StrictCenterSafe);
    auto cube = CubeState().applyStringScramble(alg);
    auto ct = cube.getCaseType(criteria);

    // follow strict center safety
    ASSERT_EQ(CaseType::caseTypeEnd, ct) << ct << cube;

    // apply 2 more times
    cube.applyStringScramble(alg).applyStringScramble(alg); // now centers are safe but not strictly
    ct = cube.getCaseType(criteria);
    ASSERT_NE(CaseType::allSolved, ct) << ct << cube;
    ASSERT_EQ(CaseType::t22swaps, ct) << ct << cube;
}

TEST(SearchCriteria, CubeMatchesCriteria2) {
    const std::string alg = "M U M` U2 M U M`"; // solved-center-safe
    SearchCriteria criteria(true, CenterSafety::IgnoreCenters);
    auto cube = CubeState().applyStringScramble(alg);
    auto ct = cube.getCaseType(criteria);
    // follow strict center safety
    ASSERT_EQ(CaseType::e3cycles, ct) << ct << cube;

    // apply 2 more times
    cube.applyStringScramble(alg).applyStringScramble(alg); // now centers are safe but not strictly
    ct = cube.getCaseType(criteria);
    /*  either   */
    bool isAllSolved = (CaseType::allSolved == ct);
    bool isT22swap = (CaseType::t22swaps == ct);
    ASSERT_TRUE(isAllSolved || isT22swap) << ct << cube;
}


////////////////////////////////////// brute force solver //////////////////////////////////////

bool canBeSolvedIn1s(const MovesArray& moves) {
    constexpr auto kSolveingLimit = 1s;
    CubeState state;

    for (auto move: moves)
        state.applyScrambleMove(move);

    bool isSolved = false;
    auto solveAsync = [&]() {
        BruteForceSolver solver(state);
        auto solution = solver.solve(4); // will run forever if this will not be solved in 1s
        isSolved = (kNoMove != solution.front());
    };
    std::thread(solveAsync).detach();
    auto startTime = std::chrono::steady_clock::now();
    while ((std::chrono::steady_clock::now() - startTime) < kSolveingLimit && !isSolved)
        std::this_thread::sleep_for(1ms);

    return isSolved;
}

TEST(SolverTests, BruteForceSolverSolvesSimpleStuff) {
    ASSERT_TRUE(canBeSolvedIn1s({0}));
    ASSERT_TRUE(canBeSolvedIn1s({1}));
}

TEST(SolverTests, CantSolveSexyIn3moves) {
    CubeState state;
    state.applyStringScramble("R U R\' U\'");
    BruteForceSolver solver(state);
    auto solution = solver.solve(3);
    ASSERT_TRUE(kNoMove == solution.front());
}

TEST(SolverTests, SemiSexyMoveSoltution) {
    CubeState state;
    state.applyStringScramble("R U R\'");
    BruteForceSolver solver(state);
    auto solution = solver.solve(3);
    ASSERT_STREQ(toString(solution).c_str(), "R U\' R\'");
}

TEST(SolverTests, CornerTwist) {
    CubeState state;
    ASSERT_FALSE(state.cornersTwistedAndSolved());
    ASSERT_FALSE(state.hasCornerTwists());
    // manually flipping corner with sexy moves
    state.applyStringScramble("R U R\' U\'");
    state.applyStringScramble("R U R\' U\'");
    state.applyStringScramble("D");
    state.applyStringScramble("U R U\' R\'");
    state.applyStringScramble("U R U\' R\'");
    state.applyStringScramble("D\'");
    ASSERT_TRUE(state.cornersTwistedAndSolved());
    ASSERT_TRUE(state.hasCornerTwists());
    ASSERT_TRUE(state.centersAreSafe());
}

TEST(SolverTests, CornerTwistFunc) {
    for (size_t i = 0; i < kNumCorners; ++i) {
        CubeState state;
        ASSERT_FALSE(state.cornersTwistedAndSolved());
        ASSERT_FALSE(state.hasCornerTwists());
        state.twistCorner(i, true); // cw
        ASSERT_TRUE(state.cornersTwistedAndSolved());
        ASSERT_TRUE(state.hasCornerTwists()) << state.toString();
        state.twistCorner(i, false); // ... back to solved
        ASSERT_FALSE(state.cornersTwistedAndSolved());
        ASSERT_FALSE(state.hasCornerTwists());
        state.twistCorner(i, false); // ccw
        ASSERT_TRUE(state.cornersTwistedAndSolved());
        ASSERT_TRUE(state.hasCornerTwists());
        state.twistCorner(i, false); // ccw second time
        state.twistCorner(i, false); // ccw third time -> solved
        ASSERT_FALSE(state.cornersTwistedAndSolved());
        ASSERT_FALSE(state.hasCornerTwists());
        ASSERT_TRUE(state.centersAreSafe()); // just in case
    }
}

TEST(SolverTests, EdgeFlip) {
    CubeState state;
    ASSERT_FALSE(state.edgesFlippedAndSolved());
    ASSERT_FALSE(state.hasEdgeFlips());
    // manually flipping corner with sexy moves
    state.applyStringScramble("M\' U M\' U M\'");
    state.applyStringScramble("U2");
    state.applyStringScramble("M U M U M");
    state.applyStringScramble("U2");
    ASSERT_TRUE(state.edgesFlippedAndSolved());
    ASSERT_TRUE(state.hasEdgeFlips());
    ASSERT_TRUE(state.centersAreSafe()) << state.toString();
}

TEST(SolverTests, EdgeFlipFunc) {
    for (size_t i = 0; i < kNumEdges; ++i) {
        CubeState state;
        ASSERT_FALSE(state.edgesFlippedAndSolved());
        ASSERT_FALSE(state.hasEdgeFlips());
        state.flipEgde(i);
        ASSERT_TRUE(state.edgesFlippedAndSolved());
        ASSERT_TRUE(state.hasEdgeFlips());
        state.flipEgde(i); // back
        ASSERT_FALSE(state.edgesFlippedAndSolved());
        ASSERT_FALSE(state.hasEdgeFlips());
        ASSERT_TRUE(state.centersAreSafe());
    }
}

////////////////////////////////////// filesystem //////////////////////////////
TEST(Filesystem, TmpDirIsAvailable) {
    const std::string path("/tmp/commfinder.txt");
    const std::string msg("hello");
    bool fileIsOk = saveToFile(path, msg, false);
    ASSERT_TRUE(fileIsOk);
    auto retreived = getFileContents(path, true);
    ASSERT_STREQ(retreived.c_str(), msg.c_str());
}

////////////////////////////////////// testalgs //////////////////////////////
TEST(TestAlgs, AllTestAlgsMap) {
    SearchCriteria criteriaAll(true, CenterSafety::SolvedCenterSafe);
    for (const auto& p: test_algs::algsForCase) {
        CaseType caseType = p.first;
        StringVec algs = p.second;
        for (const auto& alg: algs) {
            auto cube = CubeState().applyStringScramble(alg);
            ASSERT_EQ(caseType, cube.getCaseType(criteriaAll))
                    << caseType << "\n" << alg << "\n"
                    << cube;
        }
    }

}

////////////////////////////////////// commfinder //////////////////////////////
constexpr std::string_view kTmpCommfinderPath("/tmp/cf_test_suite.txt");
constexpr bool kSkipFindingCommsTest = true;
TEST(CommFinder, TmpDirIsAvailable) {
    const std::string path(kTmpCommfinderPath);
    const std::string msg("hello");
    bool fileIsOk = saveToFile(path, msg, false);
    ASSERT_TRUE(fileIsOk);
    auto retreived = getFileContents(path, true);
    ASSERT_STREQ(retreived.c_str(), msg.c_str());
}

TEST(CommFinder, WrittenAfewComms) {
    if (kSkipFindingCommsTest)
        return;
    const SearchCriteria criteriaAll(true, CenterSafety::SolvedCenterSafe);
    CommutatorFinder cf(3, criteriaAll, kTmpCommfinderPath);
    cf.find();
    auto strings = getFileContents(std::string(kTmpCommfinderPath), false);
    ASSERT_GT(strings.size(), 0);
}
