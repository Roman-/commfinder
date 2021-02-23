#ifndef CUBESTATE_H
#define CUBESTATE_H
#include <string>
#include <map>
#include <vector>
#include <array>
#include "cube_moves.h"
#include "searchcriteria.h"

/// @class CubeState describes state of 5x5 cube: corners, edges, x-centers, t-centers, wings, caps
class CubeState {
    enum CenterSafeInfo {No = 0, Yes = 1, Idk = 2};
public:
    CubeState() = default;

    // solve all elems
    CubeState& reset();

    // solve all centers
    CubeState& resetCenters();

    // returns true if all elements are on their positions
    // this means even same-colored centers are not swapped
    bool isSolved() const;

    // returns true if caps are solved and x- and t- centers are either solved
    // or their colors matches the color of the side they're located on
    bool centersAreSafe() const;

    // returns true if all centers (and caps) are strictly solved
    bool centersAreSolved() const;

    // returns true if all corners are solved except for N corners that are twisted. N > 0
    // this does not consider any other elements
    bool cornersTwistedAndSolved() const;

    // returns true if at least one corner is flipped
    // this does not consider other solved/unsolved corners nor any other elements
    bool hasCornerTwists() const;

    // returns true if all edges are solved except for N edges that are flipped. N > 0
    // this does not consider any other elements
    bool edgesFlippedAndSolved() const;

    // returns true if at least one edge is flipped
    // this does not consider other solved/unsolved edges nor any other elements
    bool hasEdgeFlips() const;

    /* scrambling*/
    CubeState& applyScramble(const MovesArray& moves);
    void applyScrambleMove(uint8_t move);

    // user-friendlyness
    CubeState& applyStringScramble(std::string scramble);

    // corner manipulation
    void twistCorner(uint8_t cornerNumber, bool clockwise = true);
    void swapCorners(uint8_t i1, uint8_t i2);

    /// \param cycle example: ULB-FUL-DFL-BDL
    void performCornersCycle(const std::vector<uint8_t> &cycle, uint8_t prime);

    // edge manipulation
    void flipEgde(uint8_t edgeNumber);
    void swapEdges(uint8_t i1, uint8_t i2);

    /// \param edge cycle example: UL-FU-DF-BD
    void performEdgesCycle(const std::vector<uint8_t> &cycle, uint8_t prime);

    /// if current cubestate is come special type specified in SearchCriteria, \returns its type
    /// otherwise \returns CaseType::caseTypeEnd
    CaseType getCaseType(const SearchCriteria& criteria) const;

    /// \returns human-readable cycles description string. Ex.: "Uf-Ur-Br"
    /// \param ignoreCentersIfCenterSafe - if true and centers are on their sides but not
    /// on their positions, don't output cycles for them
    std::string solveAndGetCycles(bool ignoreCentersIfCenterSafe = false);
    std::string whichElementsAreUnsolved() const;

    // full state string
    std::string toString() const;

    // returns number of mismatches across all elements
    uint16_t totalNumMismatches() const;

private:
    StickersArray cornersState_ = cornersStateInitial;
    StickersArray edgesState_ = edgesStateInitial;
    StickersArray xCentersState_ = xCentersStateInitial;
    StickersArray tCentersState_ = tCentersStateInitial;
    StickersArray wingsState_ = wingsStateInitial;
    CapsArray capsState_ = capsStateInitial;

    // cached result
    mutable CenterSafeInfo centersAreSafe_ = Yes;
    void reCalculateIfCentersAreSafe() const;

    static StickersArray cornersStateInitial;
    static StickersArray edgesStateInitial;
    static StickersArray xCentersStateInitial;
    static StickersArray tCentersStateInitial;
    static StickersArray wingsStateInitial;
    static CapsArray capsStateInitial;

    /// @param corner - if true, do this for corners, else do for edges
    std::string solveAndGetMultistickerCycles(std::array<uint8_t, kNumElemStickers>& state
                                      , const StringVec& config, bool corner);
};

std::ostream& operator<<(std::ostream& oss, const CubeState& cube);

#endif // CUBESTATE_H
