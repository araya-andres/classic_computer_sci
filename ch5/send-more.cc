#include "genetic-algorithm.h"
#include <algorithm>
#include <vector>

using Letters = std::vector<char>;

double fitness(const Letters& l)
{
    return .0;
}

Letters random_instance()
{
    Letters l{'S','E','N','D','M','O','R','E','Y',' ',' '};
    std::random_shuffle(l.begin(), l.end());
    return l;
}

int main()
{
    GeneticAlgorithm<Letters> ga{.0};
}
