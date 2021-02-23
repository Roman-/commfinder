#include "searchcriteria.h"
#include "easylogging++.h"

SearchCriteria::SearchCriteria(bool searchEverything, CenterSafety safety):
   cases_(size_t(CaseType::caseTypeEnd), searchEverything),
   centerSafety_(safety) {
}

bool SearchCriteria::get(CaseType c) const {
    size_t index = static_cast<size_t>(c);
    if (index >= size_t(CaseType::caseTypeEnd))
        return false;
    return cases_[index];
}

void SearchCriteria::set(CaseType c, bool value) {
    size_t index = static_cast<size_t>(c);
    LOG_IF(index >= size_t(CaseType::caseTypeEnd) || index >= cases_.size(), FATAL)
            << "criteria out of bounds: " << index;
    cases_[index] = value;
}

CenterSafety SearchCriteria::getCenterSafety() const {
    return centerSafety_;
}

std::ostream& operator<<(std::ostream& oss, const CaseType& ct) {
    switch(ct) {
        case CaseType::c3cycles: return oss << "c3cycles";
        case CaseType::e3cycles: return oss << "e3cycles";
        case CaseType::x3cycles: return oss << "x3cycles";
        case CaseType::t3cycles: return oss << "t3cycles";
        case CaseType::w3cycles: return oss << "w3cycles";
        case CaseType::c22swaps: return oss << "c22swaps";
        case CaseType::e22swaps: return oss << "e22swaps";
        case CaseType::x22swaps: return oss << "x22swaps";
        case CaseType::t22swaps: return oss << "t22swaps";
        case CaseType::w22swaps: return oss << "w22swaps";
        case CaseType::w2cycles: return oss << "w2cycles";
        case CaseType::c5cycles: return oss << "c5cycles";
        case CaseType::e5cycles: return oss << "e5cycles";
        case CaseType::x5cycles: return oss << "x5cycles";
        case CaseType::t5cycles: return oss << "t5cycles";
        case CaseType::w5cycles: return oss << "w5cycles";
        case CaseType::corner2Twists: return oss << "corner2Twists";
        case CaseType::corner3Twists: return oss << "corner3Twists";
        case CaseType::corner4Twists: return oss << "corner4Twists";
        case CaseType::edges2flips: return oss << "edges2flips";
        case CaseType::edges4flips: return oss << "edges4flips";
        case CaseType::allSolved: return oss << "allSolved";
        case CaseType::caseTypeEnd: return oss << "caseTypeEnd";
        default: return oss << "CaseType???";
    }
}

std::string toString(CaseType ct) {
    std::ostringstream oss;
    oss << ct;
    return oss.str();
}
