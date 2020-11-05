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

assert 0 "main(){return 0;}" 
assert 42 "main(){return 42;}" 
assert 21 "main(){return 5+20-4;}"
assert 41 "main(){return 12 + 34 - 5;}"
assert 40 "main(){return 8*5;}"
assert 44 "main(){return 8 * 5 + 4;}"
assert 4 "main(){return 8 * 5 / 10;}"
assert 120 "main(){return 8 * (5 + 10);}"
assert 5 "main(){return -5+10;}"
assert 1 "main(){return 1 == 1;}"
assert 0 "main(){return 1 == 2;}"
assert 0 "main(){return 1 != 1;}"
assert 1 "main(){return 1 != 2;}"
assert 1 "main(){return 2 >= 1;}"
assert 0 "main(){return 1 > 1;}"
assert 0 "main(){return 2 <= 1;}"
assert 0 "main(){return 1 < 1;}"
assert 1 "main(){return 2 < 3;}"
assert 3 "main(){a = 3;return a;}"
assert 3 "main(){z = 3;1 == 1;return z;}"
assert 3 "main(){a1_1a = 3;return a1_1a;}"
assert 3 "main(){ _1111 =  3;return _1111;}"
assert 3 "main(){aA = 3;aa = 4;return aA;}"
assert 3 "main(){a = 4;return 3;}"
assert 3 "main(){a = 4;return 3;a = 6;}"
assert 4 "main(){a = 4;return;a = 6;}"
assert 2 "main(){if (2-1) return 2; return 3;}"
assert 3 "main(){if (1 == 9) return 2; return 3;}"
assert 2 "main(){if (1 == 1) return 2; return 3;}"
assert 3 "main(){if (1 != 1) return 2; return 3;}"
assert 3 "main(){a = 4;if (a == 4)return 3;else return 2;}"
assert 2 "main(){a = 4;if (a != 4)return 3;else return 2;}"
assert 3 "main(){a = 0; while (a < 3) a = a + 1; return a;}"
assert 0 "main(){a = 0; while (a == 1) a = a + 1; return a;}"
assert 5 "main(){b = 0; for(a = 0; a < 5; a = a + 1) b = b + 1; return b;}"
assert 5 "main(){b = 0; for(a = 0; a < 5; a = a + 1) b = b + 1; return a;}"
assert 4 "main(){if (1 == 1) {b = 4; return b;}return 3;}"
assert 3 "main(){if (1 != 1) {b = 4; return b;} else {b = 3; return b;}}"
assert 8 "main(){{a = 5; b = a + 3; return b;}}"
assert 3 "main(){if (1 == 1) {if (1 != 1) a = 4; else {a = 3;return a;} } else {b = 5; return b;}}"
assert 9 "main(){a = 0;count = 0; while (a < 10) { if(a == 2){ a = a + 2; b = 0; } else { a = a + 1; } count = count + 1; } return count;}"
assert 3 "main(){return return3();}"
assert 1 "main(){return return_num1(1);}"
assert 2 "main(){return return_num2(1, 1);}"
assert 3 "main(){return return_num3(1, 1, 1);}"
assert 4 "main(){return return_num4(1, 1, 1, 1);}"
assert 5 "main(){return return_num5(1, 1, 1, 1, 1);}"
assert 6 "main(){return return_num6(1, 1, 1, 1, 1, 1);}"
assert 7 "main(){return return_num6(1 + 1, 1, 1, 1, 1, 1);}"
assert 2 "add(){return 1 + 1;} main(){return add();}"
assert 2 "add(a, b){return 1 + 1;} main(){return add(1, 1);}"
assert 1 "add(a, b){return a;} main(){return add(1, 1);}"
assert 2 "add(a, b){return a + 1;} main(){return add(1, 1);}"
assert 1 "add(a, b){return b;} main(){return add(1, 1);}"
assert 2 "add(a, b){return a + b;} main(){return add(1, 1);}"
assert 3 "add(a, b, c){return a + b + c;} main(){return add(1, 1, 1);}"
assert 4 "add(a, b, c, d){return a + b + c + d;} main(){return add(1, 1, 1, 1);}"
assert 5 "add(a, b, c, d, e){return a + b + c + d + e;} main(){return add(1, 1, 1, 1, 1);}"
assert 6 "add(a, b, c, d, e, f){return a + b + c + d + e + f;} main(){return add(1, 1, 1, 1, 1, 1);}"

echo OK
