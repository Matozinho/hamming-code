#include <bitset>
#include <iostream>
#include <string>
#include <utility>

namespace hamming {
// methods used by main
void write(char *fileName, std::string outputFileName);

void read(char *fileName, std::string outputFileName);

// auxiliary methods and structs
struct StructWord;

std::bitset<13> getBitsetFrom2Bytes(char *word);

std::bitset<13> encodeByte(std::bitset<8> word);

std::pair<std::bitset<8>, bool> decodeByte(std::bitset<13> hw);

std::string removeHwamExtension(std::string filename);

std::pair<StructWord, bool> cTest(std::bitset<13> hw);

bool gTest(std::bitset<13> hw);
} // namespace hamming
