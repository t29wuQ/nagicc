#!/bin/bash
cc -c -o tmp2 ./test/call_func.c

assert() {
  expected="$1"
  input="$2"

  ./nagicc "$input" > tmp.s
  cc -c -o tmp1 tmp.s
  cc tmp1 tmp2 -o tmp
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "int main(){return 0;}" 
assert 42 "int main(){return 42;}" 
assert 21 "int main(){return 5+20-4;}"
assert 41 "int main(){return 12 + 34 - 5;}"
assert 40 "int main(){return 8*5;}"
assert 44 "int main(){return 8 * 5 + 4;}"
assert 4 "int main(){return 8 * 5 / 10;}"
assert 120 "int main(){return 8 * (5 + 10);}"
assert 5 "int main(){return -5+10;}"
assert 1 "int main(){return 1 == 1;}"
assert 0 "int main(){return 1 == 2;}"
assert 0 "int main(){return 1 != 1;}"
assert 1 "int main(){return 1 != 2;}"
assert 1 "int main(){return 2 >= 1;}"
assert 0 "int main(){return 1 > 1;}"
assert 0 "int main(){return 2 <= 1;}"
assert 0 "int main(){return 1 < 1;}"
assert 1 "int main(){return 2 < 3;}"
assert 3 "int main(){int a = 3;return a;}"
assert 3 "int main(){int z = 3;1 == 1;return z;}"
assert 3 "int main(){int a1_1a = 3;return a1_1a;}"
assert 3 "int main(){int _1111 =  3;return _1111;}"
assert 3 "int main(){int aA = 3;int aa = 4;return aA;}"
assert 3 "int main(){int a = 4;return 3;}"
assert 3 "int main(){int a = 4;return 3;a = 6;}"
assert 4 "int main(){int a = 4;return;a = 6;}"
assert 2 "int main(){if (2-1) return 2; return 3;}"
assert 3 "int main(){if (1 == 9) return 2; return 3;}"
assert 2 "int main(){if (1 == 1) return 2; return 3;}"
assert 3 "int main(){if (1 != 1) return 2; return 3;}"
assert 3 "int main(){int a = 4;if (a == 4)return 3;else return 2;}"
assert 2 "int main(){int a = 4;if (a != 4)return 3;else return 2;}"
assert 3 "int main(){int a = 0; while (a < 3) a = a + 1; return a;}"
assert 0 "int main(){int a = 0; while (a == 1) a = a + 1; return a;}"
assert 5 "int main(){int b = 0; for(int a = 0; a < 5; a = a + 1) b = b + 1; return b;}"
assert 5 "int main(){int b = 0; for(int a = 0; a < 5; a = a + 1) b = b + 1; return a;}"
assert 4 "int main(){if (1 == 1) {int b = 4; return b;}return 3;}"
assert 3 "int main(){if (1 != 1) {int b = 4; return b;} else {b = 3; return b;}}"
assert 8 "int main(){{int a = 5; int b = a + 3; return b;}}"
assert 3 "int main(){if (1 == 1) {if (1 != 1) int a = 4; else {a = 3;return a;} } else {int b = 5; return b;}}"
assert 9 "int main(){int a = 0;int count = 0; while (a < 10) { if(a == 2){ a = a + 2; int b = 0; } else { a = a + 1; } count = count + 1; } return count;}"
assert 3 "int main(){return return3();}"
assert 1 "int main(){return return_num1(1);}"
assert 2 "int main(){return return_num2(1, 1);}"
assert 3 "int main(){return return_num3(1, 1, 1);}"
assert 4 "int main(){return return_num4(1, 1, 1, 1);}"
assert 5 "int main(){return return_num5(1, 1, 1, 1, 1);}"
assert 6 "int main(){return return_num6(1, 1, 1, 1, 1, 1);}"
assert 7 "int main(){return return_num6(1 + 1, 1, 1, 1, 1, 1);}"
assert 2 "int add(){return 1 + 1;} int main(){return add();}"
assert 2 "int add(a, b){return 1 + 1;} int main(){return add(1, 1);}"
assert 1 "int add(a, b){return a;} int main(){return add(1, 1);}"
assert 2 "int add(a, b){return a + 1;} int main(){return add(1, 1);}"
assert 1 "int add(a, b){return b;} int main(){return add(1, 1);}"
assert 2 "int add(a, b){return a + b;} int main(){return add(1, 1);}"
assert 3 "int add(a, b, c){return a + b + c;} int main(){return add(1, 1, 1);}"
assert 4 "int add(a, b, c, d){return a + b + c + d;} int main(){return add(1, 1, 1, 1);}"
assert 5 "int add(a, b, c, d, e){return a + b + c + d + e;} int main(){return add(1, 1, 1, 1, 1);}"
assert 6 "int add(a, b, c, d, e, f){return a + b + c + d + e + f;} int main(){return add(1, 1, 1, 1, 1, 1);}"
assert 3 "int main() {int x = 3; int y = &x; return *y;}"
assert 3 "int main() {int x=3; int y=&x; int z=&y; return **z;}"

echo OK
