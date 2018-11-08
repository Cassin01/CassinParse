#include "parsecpp.cpp"
#include <numeric>

int toInt(const std::string &s) {
  int ret;
  std::istringstream(s) >> ret;
  return ret;
}

auto number = apply<int, std::string>(toInt, many1(digit));

Parser<int> term = [](Source *s) {
  int x = number(s);
  auto xs = many(
    Parser<std::function<int (int)>>([](Source *s) {
      char1('*')(s);
      int x = number(s);
      return [=](int y) { return y * x; };
    }) || Parser<std::function<int (int)>>([](Source *s) {
      char1('/')(s);
      int x = number(s);
      return [=](int y) { return y / x; };
    })
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x,
    [](int x, const std::function<int (int)> &f) { return f(x); });
};


Parser<int> expr = [](Source *s) {
  int x = term(s);                                   // 項を取得
  auto xs = many(
    Parser<std::function<int (int)>>([](Source *s) {
      char1('+')(s);
      int x = term(s);                               // 項を取得
      return [=](int y) { return y + x; };
    }) || Parser<std::function<int (int)>>([](Source *s) {
      char1('-')(s);
      int x = term(s);                               // 項を取得
      return [=](int y) { return y - x; };
    })
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x,
    [](int x, const std::function<int (int)> &f) { return f(x); });
};


int main() {
  parseTest(number, "123");
  parseTest(expr,   "1+2");
  parseTest(expr,   "123");
  parseTest(expr,   "1+2+3");
  parseTest(expr,   "1-2-3");
  parseTest(expr,   "1-2+3");
  parseTest(expr,   "2*3+4"); //
  parseTest(expr,   "2+3*4");
  parseTest(expr,   "100/10/2");
}
