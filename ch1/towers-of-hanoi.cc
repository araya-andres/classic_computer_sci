// towers-of-hanoi.cc
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
#include <sstream>
#include <vector>

class Hanoi {
    private:
        std::vector<char> towerA, towerB, towerC;
        size_t numberOfDisks;
        std::string towerToString(const std::vector<char>& tower) {
            std::stringstream tempResult;
            std::string result;
            for(size_t i=0; i<tower.size(); i++) {
                tempResult << tower[i] << (i==tower.size()-1 ? "" : ", ");
                std::string temp;
                tempResult >> temp;
                result.append(temp);
            }
            return result;
        };
        void move(std::vector<char>& begin, std::vector<char>& end,
            std::vector<char>& temp, size_t n) {
            if( n==1 ) {
                char last = begin.back();
                end.push_back(last);
                begin.pop_back();
            } else {
                move(begin, temp, end, n-1);
                move(begin, end, temp, 1);
                move(temp, end, begin, n-1);
            }

        };
    public:
        Hanoi(size_t disks): numberOfDisks(disks) {
            for(size_t i=0; i<disks; i++) {
                towerA.push_back(i+49);
            }
        };
        ~Hanoi() {

        };
        void solve() {
            move(towerA, towerB, towerC, numberOfDisks);
        }
        void print() {
            std::cout << "Tower A: [" << towerToString(towerA) << "]" << std::endl;
            std::cout << "Tower B: [" << towerToString(towerB) << "]" << std::endl;
            std::cout << "Tower C: [" << towerToString(towerC) << "]" << std::endl;
        };
};

int main(int argc, char* argv[]) {

    size_t towerHeight = 3;
    if( argc>1 ) {
        std::stringstream argParser;
        argParser << argv[1];
        argParser >> towerHeight;
    }
    Hanoi game(towerHeight);
    game.print();
    game.solve();
    game.print();
}