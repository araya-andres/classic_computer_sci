#include <iostream>
#include <vector>

const unsigned MAX_NUM = 3;

enum class Side { EAST, WEST };

std::ostream& operator<<(std::ostream& os, Side s) {
    return os << (s == Side::EAST ? "east" : "west");
}

struct MCState {
    const int missionaries;
    const int cannibals;
    const Side side;
    int west_bank_missionaries() const { return missionaries; }
    int west_bank_cannibals() const { return cannibals; }
    int east_bank_missionaries() const { return MAX_NUM - missionaries; }
    int east_bank_cannibals() const { return MAX_NUM - cannibals; }
    bool is_legal() const {
        return !(west_bank_missionaries() < west_bank_cannibals() && west_bank_missionaries() > 0)
            && !(east_bank_missionaries() < east_bank_cannibals() && east_bank_missionaries() > 0);
    }
};

bool operator==(const MCState& lhs, const MCState& rhs) {
    return lhs.missionaries == rhs.missionaries
        && lhs.cannibals == rhs.cannibals
        && lhs.side == rhs.side;
}

std::ostream& operator<<(std::ostream& os, const MCState& s) {
    return os
        << "On the west bank there are " << s.missionaries << " missionaries and " << s.cannibals << " cannibals.\n"
        << "On the east bank there are " << (MAX_NUM - s.missionaries) << " missionaries and " << (MAX_NUM - s.cannibals) << " cannibals.\n"
        << "The boat is on the " << s.side << " bank\n";
}

bool goal_test(const MCState& s) {
    return s == MCState{0, 0, Side::WEST};
}

std::vector<MCState> successorsMC(const MCState& s) {
    std::vector<MCState> successors;
    auto other_side = Side::EAST;
    auto sign = -1;
    if (s.side == Side::WEST) {
        other_side = Side::WEST;
        sign = 1;
    }
    if (s.west_bank_missionaries() > 1)
        successors.push_back({s.west_bank_missionaries() + sign * 2, s.west_bank_cannibals(), other_side});
    if (s.west_bank_missionaries() > 0)
        successors.push_back({s.west_bank_missionaries() + sign * 1, s.west_bank_cannibals(), other_side});
    if (s.west_bank_cannibals() > 1)
        successors.push_back({s.west_bank_missionaries(), s.west_bank_cannibals() + sign * 2, other_side});
    if (s.west_bank_cannibals() > 0)
        successors.push_back({s.west_bank_missionaries(), s.west_bank_cannibals() + sign * 1, other_side});
    return successors;
}

int main() {
}
