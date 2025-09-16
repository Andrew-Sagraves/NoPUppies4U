// $PATH parser

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int main() {
  // we will be using popen() in our project
  const char *cmd = "echo $PATH";
  string result;
  char buffer[256];

  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    std::cerr << "popen() failed!" << std::endl;
    return 1;
  }

  while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    result += buffer; // append into std::string
  }
  pclose(pipe);

  std::cout << "PATH is:\n" << result << std::endl;
  return 0;
}
