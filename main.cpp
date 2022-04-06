#include "hamming/hamming.hpp"
#include <cstring>

using namespace std;

string exrtactFileName(char *fileName) {
  string fileNameStr(fileName);
  size_t lastSlash = fileNameStr.find_last_of("/");
  size_t lastDot = fileNameStr.find_last_of(".");

  if (lastSlash == string::npos)
    lastSlash = 0;
  else
    lastSlash++;

  if (lastDot == string::npos)
    lastDot = fileNameStr.size();

  return fileNameStr.substr(lastSlash, fileNameStr.size() - 1);
}

void showInstructions() {
  cout << "Usage: ./hamming.x <pathToFile> <options>" << endl;
  cout << "Options: " << endl;
  cout << "-R : decode a file encoded by hamming logic" << endl;
  cout << "-W : encode a file using hamming logic" << endl;
  return;
}

// argv[1] is the name of the file and argv[2] is the command (R for read or W
// for write)
int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "Error, wrong number of arguments" << endl;
    showInstructions();
    return 0;
  }

  string fileName = exrtactFileName(argv[1]);

  if (!strcmp(argv[2], "-W"))
    hamming::write(argv[1], fileName);
  else if (!strcmp(argv[2], "-R"))
    hamming::read(argv[1], fileName);
  else {
    cout << "Error: Invalid Option" << endl;
    showInstructions();
  }
  return 0;
}
