#include "user/helperFunctions.h"

// define: isAlpha()
int isAlpha(char ch) {

  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
} // end of isAlpha()

// define: isNum()
int isNum(char ch) { return (ch >= '0' && ch <= '9'); } //  end of isNum()

// define: isAlphaNum()
int isAlphaNum(char ch) {

  return (isAlpha(ch) || isNum(ch));
} // end of isAlphaNum()

// define: isChar()
int isChar(char ch) {

  return (ch != '-' && ch != '_' && ch != '+' && ch != '%' && ch != '@' &&
          ch != '/' && ch != '.' && ch != ',' && isAlphaNum(ch) == 0);
} // end of isChar()

// define; stringCompare()
int stringCompare(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
} // end of stringCompare()

// define: stringToInteger()
int stringToInteger(const char *s) {
  int sign = 1;
  int result = 0;

  while (*s == ' ' || *s == '\t' || *s == '\n') {
      s++;
  }

  if (*s == '-') {
      sign = -1;
      s++;
  } else if (*s == '+') {
      s++;
  }

  while (*s >= '0' && *s <= '9') {
      result = result * 10 + (*s - '0');
      s++;
  }

  return sign * result;
} // end of stringToInteger()
