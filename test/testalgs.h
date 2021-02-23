/// @file testalgs.h contains algorithms for special cases so that we can test
/// these algorithms on the actual cube
#ifndef TESTALGS_H
#define TESTALGS_H

#include <string>
#include <map>

#include "helpers.h"
#include "cube_moves.h"
#include "searchcriteria.h"
namespace test_algs {

static const std::string kSexy("R U R` U`");
static const std::string k2sexy(kSexy + " " + kSexy);
static const std::string k4sexy("U R U` R` U R U` R`");
static const std::string k2eflipUfBu("M` U' M` U` M` U2 M U` M U` M U2");
// all of these algs are SolvedCenterSafe
const std::map<CaseType, StringVec> algsForCase = {
    {CaseType::c3cycles,{"R U R` D R U` R` D`", "U` L2 U R2 U` L2 U R2"}},
    {CaseType::e3cycles,{"M` U L` U` M U L U`", "M` U2 M U2"}},
    {CaseType::x3cycles,{"U l u l` U` l u` l`", "r` d2 r U2 r` d2 r U2"}},
    {CaseType::t3cycles,{"M` U l` U` M U l U`", "E r U` r` E` r U r`"}},
    {CaseType::w3cycles,{"l` U L` U` l U L U`", "d R U` R` d` R U R`"}},
    {CaseType::c22swaps,{"M2 l2 r2 U M2 l2 r2 U2 M2 l2 r2 U M2 l2 r2 U2"}},
    {CaseType::e22swaps,{"M2 U M2 U2 M2 U M2"}},
    {CaseType::x22swaps,{"l f l` u l` u` l f`"}},
    {CaseType::t22swaps,{"M` U2 M U2 M` U2 M U2 M` U2 M U2"}},
    {CaseType::w22swaps,{"R L f R` L` F2 R L f` R` L` F2"}},
    {CaseType::w2cycles,{"r U2 r U2 l` M` r U2 r U2 r` U2 l U2 r2 M"}},
    {CaseType::c5cycles,{"U R` L D2 R L` U` R` L D2 R L`"}},
    {CaseType::e5cycles,{"M' U2 M U M' U2 M U'"}},
    {CaseType::x5cycles,{"l u L u l` u` L` u`"}},
    {CaseType::t5cycles,{"M u L u M` u` L` u`"}},
    {CaseType::w5cycles,{"L D U' l D' U L` U` D l` U D`"}},
    {CaseType::corner2Twists,{k2sexy+" D "+k4sexy+" D`"}},
    {CaseType::corner3Twists,{k2sexy+" D "+k2sexy+" D "+k2sexy+" D2"}},
    {CaseType::corner4Twists,{k2sexy+" D "+k4sexy+" D "+k2sexy+" D "+k4sexy+" D"}},
    {CaseType::edges2flips,{"R M U2 M2 U R` U` M2 U2 M` U` R U R`", k2eflipUfBu}},
    {CaseType::edges4flips,{k2eflipUfBu + " U " + k2eflipUfBu + " U`"}},
    {CaseType::allSolved,{"", k2sexy + " " + k2sexy + " " + k2sexy, "R R` U U`", "U2 U2"}},
    {CaseType::caseTypeEnd, {"M U M` U M U M` U", "R", "S"}}
};

}

#endif // TESTALGS_H
