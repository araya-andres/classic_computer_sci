#include "bfs.h"
#include "range/v3/all.hpp"
#include <iostream>
#include <vector>

const unsigned MAX_NUM = 3;

enum class Side { EAST, WEST };

std::ostream& operator<<(std::ostream& os, Side s) {
    return os << (s == Side::WEST ? "west" : "east");
}

struct MCState {
    const int missionaries_;
    const int cannibals_;
    const Side side_;
    int missionaries() const {
        return side_ == Side::WEST
            ? west_bank_missionaries()
            : east_bank_missionaries();
    }
    int cannibals() const {
        return side_ == Side::WEST
            ? west_bank_cannibals()
            : east_bank_cannibals();
    }
    Side side() const { return side_; }
    int west_bank_missionaries() const { return missionaries_; }
    int west_bank_cannibals() const { return cannibals_; }
    int east_bank_missionaries() const { return MAX_NUM - missionaries_; }
    int east_bank_cannibals() const { return MAX_NUM - cannibals_; }
    Side other_side() const {
        return side_ == Side::WEST
            ? Side::EAST
            : Side::WEST;
    }
    bool is_legal() const {
        return !(west_bank_missionaries() < west_bank_cannibals() && west_bank_missionaries() > 0)
            && !(east_bank_missionaries() < east_bank_cannibals() && east_bank_missionaries() > 0);
    }
};

bool operator==(const MCState& lhs, const MCState& rhs) {
    return lhs.missionaries_ == rhs.missionaries_
        && lhs.cannibals_ == rhs.cannibals_
        && lhs.side_ == rhs.side_;
}

bool operator<(const MCState& lhs, const MCState& rhs) {
    if (lhs.side() == Side::WEST && rhs.side() == Side::EAST) {
        return true;
    }
    if (lhs.side() == Side::EAST && rhs.side() == Side::WEST) {
        return false;
    }
    return lhs.west_bank_missionaries() < rhs.west_bank_missionaries()
        || (lhs.west_bank_missionaries() == rhs.west_bank_missionaries() && lhs.west_bank_cannibals() < rhs.west_bank_cannibals());
}

std::ostream& operator<<(std::ostream& os, const MCState& s) {
    return os
        << "On the west bank there are " << s.west_bank_missionaries()
        << " missionaries and " << s.west_bank_cannibals() << " cannibals.\n"
        << "On the east bank there are " << s.east_bank_missionaries()
        << " missionaries and " << s.east_bank_cannibals() << " cannibals.\n"
        << "The boat is on the " << s.side() << " bank";
}

bool goal_test(const MCState& s) {
    return s.west_bank_missionaries() == 0 && s.west_bank_cannibals() == 0;
}

std::vector<MCState> successorsMC(const MCState& s) {
    using namespace ranges::view;
    std::vector<MCState> successors;
    auto sign = (s.side() == Side::WEST) ? -1 : 1;
    if (s.missionaries() > 1)
        successors.push_back({s.west_bank_missionaries() + sign * 2, s.west_bank_cannibals(), s.other_side()});
    if (s.missionaries() > 0)
        successors.push_back({s.west_bank_missionaries() + sign * 1, s.west_bank_cannibals(), s.other_side()});
    if (s.cannibals() > 1)
        successors.push_back({s.west_bank_missionaries(), s.west_bank_cannibals() + sign * 2, s.other_side()});
    if (s.cannibals() > 0)
        successors.push_back({s.west_bank_missionaries(), s.west_bank_cannibals() + sign * 1, s.other_side()});
    if (s.missionaries() > 0 && s.cannibals() > 0)
        successors.push_back({s.west_bank_missionaries() + sign * 1, s.west_bank_cannibals() + sign * 1, s.other_side()});
    return successors | remove_if([](const MCState& s) { return !s.is_legal(); });
}

void print_solution(const Path<MCState>& p) {
    if (p.empty()) {
        std::cout << "no solution found\n";
        return;
    }
    auto old_state = p.begin();
    auto current_state = old_state + 1;
    std::cout << *old_state << '\n';
    while (current_state != p.end()) {
        int missionaries = old_state->missionaries(), cannibals = old_state->cannibals();
        if (current_state->side() == Side::WEST) {
            missionaries -= current_state->east_bank_missionaries();
            cannibals -= current_state->east_bank_cannibals();
        } else {
            missionaries -= current_state->west_bank_missionaries();
            cannibals -= current_state->west_bank_cannibals();
        }
        std::cout
            << "***\n"
            << missionaries << " missionaries and " << cannibals << " cannivals"
            << " moved from the " << old_state->side() << " bank"
            << " to the " << current_state->side() << " bank.\n"
            << *current_state << '\n';
        ++old_state, ++current_state;
    }
}

int main() {
    print_solution(bfs<MCState>(MCState{3, 3, Side::WEST}, goal_test, successorsMC));
}
