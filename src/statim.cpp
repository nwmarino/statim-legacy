#include <string>

#include "./lex/lexer.h"

using std::string;

int main() {
  string path("../test/function/main.statim");
  Lexer lex(path);
}
