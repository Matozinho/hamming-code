#include "hamming.hpp"
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

using namespace std;

// verify of is pow of 2
bool isPowOf2(int n) { return (n & (n - 1)) == 0; }

string hamming::removeHwamExtension(string filename) {
  size_t lastDot = filename.find_last_of(".");
  if (lastDot == string::npos)
    return filename;

  return filename.substr(0, lastDot);
}

struct hamming::StructWord {
  bitset<8> M;
  bitset<4> C;
  bitset<1> G;

  void setStruct(bitset<13> hw) {
    G[0] = hw[0];
    int cIdx = 0;
    int mIdx = 0;

    for (int i = 1; i < 13; i++) {
      if (isPowOf2(i))
        C[cIdx++] = hw[i];
      else
        M[mIdx++] = hw[i];
    }
  }

  bitset<13> getStructuredHW() {
    bitset<13> hw;
    hw[0] = G[0];
    int cIdx = 0;
    int mIdx = 0;

    for (int i = 1; i < 13; i++) {
      isPowOf2(i) ? hw[i] = C[cIdx++] : hw[i] = M[mIdx++];
    }
    return hw;
  }
};

bitset<13> hamming::encodeByte(bitset<8> word) {
  hamming::StructWord hw;
  hw.M = word;

  hw.C.set(0, (word[6] ^ word[4] ^ word[3] ^ word[1] ^ word[0]));
  hw.C.set(1, (word[6] ^ word[5] ^ word[3] ^ word[2] ^ word[0]));
  hw.C.set(2, (word[7] ^ word[3] ^ word[2] ^ word[1]));
  hw.C.set(3, (word[7] ^ word[6] ^ word[5] ^ word[4]));

  hw.G = (word[7] ^ word[6] ^ word[5] ^ word[4] ^ hw.C[3] ^ word[3] ^ word[2] ^
          word[1] ^ hw.C[2] ^ word[0] ^ hw.C[1] ^ hw.C[0]);

  bitset<13> structuredHW = hw.getStructuredHW();

  return structuredHW;
}

// the boolean means error: if is false, then the word is correct
pair<hamming::StructWord, bool> hamming::cTest(std::bitset<13> hw) {
  hamming::StructWord hammingWord;
  hammingWord.setStruct(hw);

  bitset<13> bitsetTest = hamming::encodeByte(hammingWord.M);

  hamming::StructWord hammingWordTest;
  hammingWordTest.setStruct(bitsetTest);

  bitset<4> syndromeWord = hammingWord.C ^ hammingWordTest.C;

  if (syndromeWord.to_ulong() == 0)
    return {hammingWord, false};

  // impossible fix the word
  if (syndromeWord.to_ulong() > 12) {
    cout << "syndrome word value is bigger than 12" << endl;
    return {hamming::StructWord{}, true};
  }

  hw.flip(syndromeWord.to_ulong());
  hamming::StructWord fixedWord;
  fixedWord.setStruct(hw);

  return {fixedWord, false};
}

// return true if the word is correct
bool hamming::gTest(std::bitset<13> hw) {
  hamming::StructWord hammingWord;
  hammingWord.setStruct(hw);

  bitset<13> hammingTest = hamming::encodeByte(hammingWord.M);

  bitset<1> gTest = hammingTest[0] ^ hw[0];
  return (gTest.to_ulong() == 0);
}

// the boolean means error: if is false, then the word is correct
pair<bitset<8>, bool> hamming::decodeByte(bitset<13> hw) {
  bitset<8> decodedWord;

  pair<hamming::StructWord, bool> cCorrection = hamming::cTest(hw);

  if (cCorrection.second)
    return {decodedWord, true};

  hamming::StructWord hammingWord = cCorrection.first;

  if (!gTest(hammingWord.getStructuredHW())) {
    cout << "g is incompatible" << endl;
    return {decodedWord, true};
  }

  return {hammingWord.M, false};
}

bitset<13> hamming::getBitsetFrom2Bytes(char *word) {
  bitset<13> finalBitset;
  bitset<8> firstByte(word[0]);
  bitset<8> secondByte(word[1]);

  for (int i = 0; i < 8; i++)
    finalBitset.set(i, firstByte[i]);

  for (int i = 8; i < 13; i++)
    finalBitset.set(i, secondByte[i - 8]);

  return finalBitset;
}

// read 16 bits from file and convert into 8 bits word
void hamming::read(char *fileName, string outputFileName) {
  ifstream inputFile;
  ofstream outputFile;
  char dataBuffer[2];
  string outputFileNameWithoutHwam = removeHwamExtension(outputFileName);

  cout << "start reading..." << endl;

  inputFile.open(fileName, ios::binary | ios::in);
  outputFile.open(outputFileNameWithoutHwam, ios::binary | ios::out);

  if (!inputFile.is_open() || !outputFile.is_open())
    throw "Error opening file";

  while (!inputFile.eof()) {
    inputFile.read((char *)&dataBuffer, 2);
    if (inputFile.eof())
      break;

    bitset<13> hw = getBitsetFrom2Bytes(dataBuffer);

    pair<bitset<8>, bool> decodeResult = hamming::decodeByte(hw);
    if (decodeResult.second) {
      cout << "corrupted word, impossible to recover!" << endl;
      continue;
    }

    bitset<8> decodedWord = decodeResult.first;
    outputFile.write((char *)&decodedWord, sizeof(char));
  }
  outputFile.close();
  inputFile.close();

  cout << "end reading, file \"" << outputFileNameWithoutHwam << "\" generated!"
       << endl;
}

void hamming::write(char *fileName, std::string outputFileName) {
  ifstream inputFile;
  ofstream outputFile;
  char dataBuffer;
  bitset<8> bs;

  cout << "start writing..." << endl;

  inputFile.open(fileName, ios::binary | ios::in);
  outputFile.open(outputFileName + ".hwam", ios::binary | ios::out);

  if (!inputFile.is_open() || !outputFile.is_open())
    throw "Error opening file";

  while (!inputFile.eof()) {
    inputFile.read(reinterpret_cast<char *>(&dataBuffer), 1);
    if (inputFile.eof())
      break;

    bs = dataBuffer;

    bitset<13> hw = hamming::encodeByte(bs);
    outputFile.write((char *)&hw, 2);
  }

  inputFile.close();
  outputFile.close();

  cout << "end writing!, file \"" << outputFileName << ".hwam\" generated"
       << endl;
}
