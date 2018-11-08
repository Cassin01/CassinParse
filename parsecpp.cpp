#include <iostream>
#include <string>
#include <functional>

using Source = const char *;

template <typename T>
using Parser  = std::function<T (Source *)>;
// T: 戻り値, (...): 引数

template <typename T>
void parseTest(const Parser<T> &p, const Source &src) {
  Source s = src;
  try {
    std::cout << p(&s) << std::endl;
  } catch (const char *e) {
    std::cout << e << std::endl;
  }
}


Parser<char> satisfy(const std::function<bool (char)> &f) {
  return [=](Source *s) {
    char ch = **s;
    if (!ch) throw "too short";
    if (!f(ch)) throw "not satisfy";
    ++*s;
    return ch;
  };
}

// satisfy より低く
auto anyChar = satisfy([](char) { return true; });

Parser<char> char1(char ch) {
  return satisfy([=](char c) { return c == ch; });
}

bool isDigit   (char ch) { return '0' <= ch && ch <= '9'; }
bool isUpper   (char ch) { return 'A' <= ch && ch <= 'Z'; }
bool isLower   (char ch) { return 'a' <= ch && ch <= 'z'; }
bool isAlpha   (char ch) { return isUpper(ch) || isLower(ch); }
bool isAlphaNum(char ch) { return isAlpha(ch) || isDigit(ch); }
bool isLetter  (char ch) { return isAlpha(ch) || ch == '_';   }

auto digit    = satisfy(isDigit   );
auto upper    = satisfy(isUpper   );
auto lower    = satisfy(isLower   );
auto alpha    = satisfy(isAlpha   );
auto alphaNum = satisfy(isAlphaNum);
auto letter   = satisfy(isLetter  );

template <typename T1, typename T2>
Parser<std::string> operator+(const Parser<T1> &x, const Parser<T2> &y) {
  return [=](Source *s) {
    std::string ret;
    ret += x(s);
    ret += y(s);
    return ret;
  };
}

template <typename T>
Parser<std::string> operator*(int n, const Parser<T> &x) {
  return [=](Source *s) {
    std::string ret;
    for (int i = 0; i < n; ++i) {
      ret += x(s);
    }
    return ret;
  };
}
template <typename T>
Parser<std::string> operator*(const Parser<T> &x, int n) {
  return n * x;
}

template <typename T>
Parser<std::string> many(const Parser<T> &p) {
  return [=](Source *s) {
    std::string ret;
    try {
      for (;;) ret += p(s);
    } catch (const char *) {}
    return ret;
  };
}

template <typename T>
const Parser<T> operator||(const Parser<T> &p1, const Parser<T> &p2) {
  return [=](Source *s) {
    T ret;
    Source bak = *s;
    try {
      ret = p1(s);
    } catch (const char *) {
      if (*s != bak) throw;
      ret = p2(s);
    }
    return ret;
  };
}

template <typename T>
Parser<T> tryp(const Parser<T> &p) {
  return [=](Source *s) {
    T ret;
    Source bak = *s;
    try {
      ret = p(s);
    } catch (const char *) {
      *s = bak;
      throw;
    }
    return ret;
  };
}

Parser<std::string> string(const std::string &str) {
  return [=](Source *s) {
    for (auto it = str.begin(); it != str.end(); ++it) {
      char1(*it)(s);
    }
    return str;
  };
}
