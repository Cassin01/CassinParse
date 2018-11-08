#include "parsecpp.cpp"
#include <numeric>

int toInt(const std::string &s) {
  int ret;
  std::istringstream(s) >> ret;
  return ret;
}

/*
Parser<int> number = [](Source *s) {
  return toInt(many1(digit)(s));
};
*/

auto number = apply<int, std::string>(toInt, many1(digit));


Parser<int> expr = [](Source *s) {
  int x = number(s);
  auto xs = many(
        char1('+') >>  number
    ||  char1('-') >> -number
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x);
};


int main() {
  parseTest(number, "123");
  parseTest(expr,   "1+2");
  parseTest(expr,   "123");
  parseTest(expr,   "1+2+3");
  parseTest(expr,   "1-2-3");
  parseTest(expr,   "1-2+3");
}
