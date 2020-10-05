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

assert 0 0
assert 42 42
assert 21 "5+20-4"
assert 41 "12 + 34 - 5"
assert 40 "8*5"
assert 44 "8 * 5 + 4"
assert 4 "8 * 5 / 10"
assert 120 "8 * (5 + 10)"

echo OK
