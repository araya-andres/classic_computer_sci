// unbreakable-encryption.cc
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
#include <vector>
#include <random>

typedef unsigned char byte;
typedef std::vector<byte> byte_vector;
typedef std::pair< byte_vector, byte_vector > key_pair;

byte_vector randomKey(size_t length) {
    byte_vector result(length);
    std::random_device rd;
    for(size_t i=0; i<length; i++) {
        result[i] = rd();
    }
    return result;
};

key_pair encrypt(const std::string &original) {
    const byte_vector key = randomKey(original.size());
    byte_vector encrypted(original.size());

    for(size_t i=0; i<original.size(); i++) {
        encrypted[i] = key[i] ^ original[i];
    }

    return std::make_pair(encrypted, key);
};

std::string decrypt(key_pair &keyPair) {
    std::string result(keyPair.first.size(),' ');
    for(size_t i=0; i<result.size(); i++) {
        result[i] = keyPair.first[i] ^ keyPair.second[i];
    }
    return result;
}

int main(int argc, char* argv[]) {
    const std::string textToEncrypt = argc == 1 ? "One Time Pad" : argv[1];

    key_pair keyPair = encrypt(textToEncrypt);

    std::cout << decrypt(keyPair) << std::endl;
}