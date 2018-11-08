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


/*
Parser<int> expr = [](Source *s) {
  int x = number(s);
  auto xs = many(
        char1('+') >>  number
    ||  char1('-') >> -number
  )(s);
  return std::accumulate(xs.begin(), xs.end(), x);
};
*/

Parser<int> expr = [](Source *s) {
  int x = number(s);
  auto xs = many(
    Parser<std::function<int (int)>>([](Source *s) {// 型を修正
      char1('+')(s); 
      int x = number(s);                            // キャプチャされる変数
      return [=](int y) { return y + x; };          // クロージャを返す
    }) || Parser<std::function<int (int)>>([](Source *s) {
      char1('-')(s); 
      int x = number(s);                            // キャプチャされる変数
      return [=](int y) { return y - x; };          // クロージャを返す
    }) || Parser<std::function<int (int)>>([](Source *s) { // 追加: 掛け算
      char1('*')(s); 
      int x = number(s);                            // キャプチャされる変数
      return [=](int y) { return y * x; };          // クロージャを返す
    }) || Parser<std::function<int (int)>>([](Source *s) { // 追加: 割り算
      char1('/')(s); 
      int x = number(s);                            // キャプチャされる変数
      return [=](int y) { return y / x; };          // クロージャを返す
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
