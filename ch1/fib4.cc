// fib4.cc
//
// Adapted From Classic Computer Science Problems in Python/Java Chapter 1
// Copyright 2024 Markus Peter
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <string>

unsigned fib(unsigned n) {
    unsigned last = 0, next = 1;
    for(int i = 0; i<n; i++) {
        unsigned oldLast = last;
        last = next;
        next = oldLast + last;
    }
    return last;
};

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::cout << fib(std::stoi(argv[i])) << std::endl;
    }
}