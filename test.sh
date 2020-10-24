#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./nagicc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "return 0;" 
assert 42 "return 42;" 
assert 21 "return 5+20-4;"
assert 41 "return 12 + 34 - 5;"
assert 40 "return 8*5;"
assert 44 "return 8 * 5 + 4;"
assert 4 "return 8 * 5 / 10;"
assert 120 "return 8 * (5 + 10);"
assert 5 "return -5+10;"
assert 1 "return 1 == 1;"
assert 0 "return 1 == 2;"
assert 0 "return 1 != 1;"
assert 1 "return 1 != 2;"
assert 1 "return 2 >= 1;"
assert 0 "return 1 > 1;"
assert 0 "return 2 <= 1;"
assert 0 "return 1 < 1;"
assert 1 "return 2 < 3;"
assert 3 "a = 3;return a;"
assert 3 "z = 3;1 == 1;return z;"
assert 3 "a1_1a = 3;return a1_1a;"
assert 3 " _1111 =  3;return _1111;"
assert 3 "aA = 3;aa = 4;return aA;"
assert 3 "a = 4;return 3;"
assert 3 "a = 4;return 3;a = 6;"
assert 4 "a = 4;return;a = 6;"
assert 2 "if (2-1) return 2; return 3;"
assert 3 "if (1 == 9) return 2; return 3;"
assert 2 "if (1 == 1) return 2; return 3;"
assert 3 "if (1 != 1) return 2; return 3;"
assert 3 "a = 4;if (a == 4)return 3;else return 2;"
assert 2 "a = 4;if (a != 4)return 3;else return 2;"
assert 3 "a = 0; while (a < 3) a = a + 1; return a;"
assert 0 "a = 0; while (a == 1) a = a + 1; return a;"
assert 5 "b = 0; for(a = 0; a < 5; a = a + 1) b = b + 1; return b;"
assert 5 "b = 0; for(a = 0; a < 5; a = a + 1) b = b + 1; return a;"
assert 4 "if (1 == 1) {b = 4; return b;}return 3;"
assert 3 "if (1 != 1) {b = 4; return b;} else {b = 3; return b;}"
assert 8 "{a = 5; b = a + 3; return b;}"
assert 3 "if (1 == 1) {if (1 != 1) a = 4; else {a = 3;return a;} } else {b = 5; return b;}"
assert 9 "a = 0;count = 0; while (a < 10) { if(a == 2){ a = a + 2; b = 0; } else { a = a + 1; } count = count + 1; } return count;"

echo OK
