// fib5.cc
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

class Fib: public std::iterator<std::input_iterator_tag, unsigned, unsigned, const unsigned*, unsigned> {
    private:
        unsigned last = 0, next = 1;
    public:
        Fib() {};
        Fib& operator++() {
            unsigned oldLast = last;
            last = next;
            next = oldLast + last;
            return *this;
        };
        Fib operator++(int) {
            Fib result = *this;
            ++(*this);
            return result;
        };
        reference operator*() const { return last; };
        bool operator==(const Fib& other) const { return (last == other.last) && (next == other.next); };
        bool operator!=(const Fib& other) const { return !(*this == other); };

};


int main(int argc, char* argv[]) {
    Fib generator;
    
    for(int i = 0; i < 40; i++) {
        std::cout << i << ": " << *generator++ << std::endl;
    }
}