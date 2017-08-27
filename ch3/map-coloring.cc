#include "csp.h"
#include "prettyprint.hpp"
#include <iostream>

const char* WA  = "Western Australia";
const char* NT  = "Northern Territory";
const char* SA  = "South Australia";
const char* QLD = "Queensland";
const char* NSW = "New South Wales";
const char* VIC = "Victoria";
const char* TAS = "Tasmania";

const char* RED = "red";
const char* BLUE = "blue";
const char* GREEN = "green";

int main() {
    using Constraint = BinaryConstraint<const char*, const char*>;
    Variables<const char *> variables{WA, NT, SA, QLD, NSW, VIC, TAS};
    Domains<const char*, const char*> domains;
    for (auto v : variables) domains[v] = {RED, BLUE, GREEN};
    Constraints<Constraint> constraints{
        {WA,NT},{WA,SA},
        {NT,SA},{NT,QLD},
        {SA,QLD},{SA,NSW},{SA,VIC},
        {QLD,NSW},
        {NSW,VIC},
    };
    CSP<Constraint, const char *, const char*> csp{constraints, domains, variables};
    std::cout << backtracking_search<Constraint, const char *, const char*>(csp) << '\n';
}
