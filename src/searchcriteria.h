#ifndef SEARCHCRITERIA_H
#define SEARCHCRITERIA_H

#include <vector>
#include <iostream>

enum class CenterSafety {
    IgnoreCenters,    // [U, M U' M] allowed,     [M, U2] allowed
    SolvedCenterSafe, // [U, M U' M] NOT allowed, [M, U2] allowed
    StrictCenterSafe, // [U, M U' M] NOT allowed, [M, U2] NOT allowed
};

enum class CaseType {
    c3cycles, e3cycles, x3cycles, t3cycles, w3cycles,
    c22swaps, e22swaps, x22swaps, t22swaps, w22swaps, w2cycles,
    c5cycles, e5cycles, x5cycles, t5cycles, w5cycles,

    corner2Twists, corner3Twists, corner4Twists,
    edges2flips,   edges4flips,

    allSolved, caseTypeEnd
};

std::ostream& operator<<(std::ostream& os, const CaseType& ct);
std::string toString(CaseType ct);

/// @returns true if @param ct related to centers
inline bool isCenterCaseType(CaseType ct) {
    return ct == CaseType::x3cycles || ct == CaseType::t3cycles
           || ct == CaseType::x22swaps || ct == CaseType::t22swaps
           || ct == CaseType::x5cycles || ct == CaseType::t5cycles;
}

// elements: cextw
class SearchCriteria {
public:
    // searchEverything - set all values (except for allSolved) to true/false on initialization
    SearchCriteria(bool searchEverything, CenterSafety safety = CenterSafety::SolvedCenterSafe);

    // true if we're searching CaseType c
    bool get(CaseType c) const;

    // activate/deactivate CaseType c
    void set(CaseType c, bool value);

    CenterSafety getCenterSafety() const;

private:
    // cases[i] = true if CaseType#i is active
    std::vector<bool> cases_;

    CenterSafety centerSafety_;
};

#endif // SEARCHCRITERIA_H
