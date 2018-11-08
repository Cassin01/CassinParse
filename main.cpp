#include "parsecpp.cpp"
#include <numeric>

int toInt(const std::string &s) {
  int ret;
  std::istringstream(s) >> ret;
  return ret;
}

Parser<std::function<int (int)>> apply( // 特殊化
  const std::function<int (int, int)> &f, const Parser<int> &p) {
  return apply<int, int, int>(f, p);
}

auto number = apply<int, std::string>(toInt, many1(digit));

/*
Parser<int> term = [](Source *s) {
  int x = number(s);
  auto xs = many(
    Parser<std::function<int (int)>>([](Source *s) {
      char1('*')(s);
      return apply([](int x, int y) { return y * x; }, number)(s);
    }) || Parser<std::function<int (int)>>([](Source *s) {
      char1('/')(s);
      return apply([](int x, int y) { return y / x; }, number)(s);
    })
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x,
    [](int x, const std::function<int (int)> &f) { return f(x); });
};
*/
Parser<int> term = [](Source *s) {
  int x = number(s);
  auto xs = many(
       char1('*') >> apply([](int x, int y) { return y * x; }, number)
    || char1('/') >> apply([](int x, int y) { return y / x; }, number)
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x,
    [](int x, const std::function<int (int)> &f) { return f(x); });
};


/*
Parser<int> expr = [](Source *s) {
  int x = term(s);                                   // 項を取得
  auto xs = many(
    Parser<std::function<int (int)>>([](Source *s) {
      char1('+')(s);
      return apply([](int x, int y) { return y + x; }, term)(s);
    }) || Parser<std::function<int (int)>>([](Source *s) {
      char1('-')(s);
      return apply([](int x, int y) { return y - x; }, term)(s);
    })
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x,
    [](int x, const std::function<int (int)> &f) { return f(x); });
};
*/

Parser<int> expr = [](Source *s) {
  int x = term(s);
  auto xs = many(
       char1('+') >> apply([](int x, int y) { return y + x; }, term)
    || char1('-') >> apply([](int x, int y) { return y - x; }, term)
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
