#include <cassert>
#include <sstream>
#include <string>
#include <vector>

std::vector<bool> compress(const std::string& gene) {
    std::vector<bool> bitset;
    bitset.reserve(gene.size() * 2);
    for (auto const nucleotide : gene) {
        if (nucleotide == 'A') {
            bitset.push_back(0);
            bitset.push_back(0);
        } else if (nucleotide == 'C') {
            bitset.push_back(0);
            bitset.push_back(1);
        } else if (nucleotide == 'G') {
            bitset.push_back(1);
            bitset.push_back(0);
        } else if (nucleotide == 'T') {
            bitset.push_back(1);
            bitset.push_back(1);
        }
    }
    return bitset;
}

std::string decompress(const std::vector<bool>& bitset) {
    std::ostringstream oss;
    for (int i = 0; i < bitset.size(); i += 2) {
        const auto nucleotide = 2 * bitset[i] + bitset[i + 1];
        if (nucleotide == 0) oss << 'A';
        else if (nucleotide == 1) oss << 'C';
        else if (nucleotide == 2) oss << 'G';
        else if (nucleotide == 3) oss << 'T';
    }
    return oss.str();
}

int main(int argc, char *argv[]) {
    if (argc == 1) return 1;
    std::string gene{argv[1]};
    assert(gene == decompress(compress(gene)));
}
