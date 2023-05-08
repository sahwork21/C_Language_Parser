#!/bin/bash
# This is a shell script to check your program(s) on test cases.
# It usese the same language you normally use to type commands in
# a terminal window.  You can write whole programs in shell.

# Assume we've succeeded until we see otherwise.
FAIL=0

# Print an error message and set the fail flag.
fail() {
    echo "**** $1"
    FAIL=1
}

# Check the exit status.  The actual exit status (ASTATUS) should match
# the expected status (ESTATUS)
checkStatus() {
  ESTATUS="$1"
  ASTATUS="$2"

  # Make sure the program exited with the right exit status.
  if [ "$ASTATUS" -ne "$ESTATUS" ]; then
      fail "FAILED - incorrect program exit status. (expected $ESTATUS,  Got: $ASTATUS)"
      return 1
  fi

  return 0
}

# Check the contents of an a file.  If the expected file (EFILE)
# exists, then the actual file (AFILE) should exist and it should match.
checkFile() {
  NAME="$1"
  EFILE="$2"
  AFILE="$3"

  # Make sure we're really expecting this file.
  if [ ! -f "$EFILE" ]; then
      return 0
  fi

  # Make sure the output matches the expected output.
  echo "   diff $EFILE $AFILE"
  diff -q "$EFILE" "$AFILE" >/dev/null 2>&1
  if [ $? -ne 0 ]; then
      fail "FAILED - $NAME ($AFILE) doesn't match expected ($EFILE)"
      return 1
  fi

  return 0
}

# Same as checkFile, but if the expected file (EFILE) doesn't exist, the
# actual file (AFILE) should be empty.
checkFileOrEmpty() {
  NAME="$1"
  EFILE="$2"
  AFILE="$3"
  
  # if the expected output file doesn't exist, the actual file should be
  # empty.
  if [ ! -f "$EFILE" ]; then
      if [ -s "$AFILE" ]; then
	  fail "FAILED - $NAME ($AFILE) should be empty."
	  return 1
      fi
      return 0
  fi

  # Make sure the output matches the expected output.
  echo "   diff $EFILE $AFILE"
  diff -q "$EFILE" "$AFILE" >/dev/null 2>&1
  if [ $? -ne 0 ]; then
      fail "FAILED - $NAME ($AFILE) doesn't match expected ($EFILE)"
      return 1
  fi

  return 0
}

# The given file should exist but should be empty.
checkEmpty() {
  NAME="$1"
  AFILE="$2"
  
  if [ -s "$AFILE" ]; then
      fail "FAILED - $NAME ($AFILE) should be empty."
      return 1
  fi

  return 0
}

# Test one execution of the interpreter
testInterpreter() {
  TESTNO=$1
  ESTATUS=$2

  echo "Test $TESTNO"
  rm -f output.txt stderr.txt

  echo "   ./interpret prog-$TESTNO.txt > output.txt 2> stderr.txt"
  ./interpret prog-$TESTNO.txt > output.txt 2> stderr.txt
  ASTATUS=$?

  if ! checkStatus "$ESTATUS" "$ASTATUS" ||
     ! checkFile "Stdout output" "expected-$TESTNO.txt" "output.txt" ||
     ! checkFileOrEmpty "Stderr output" "message-$TESTNO.txt" "stderr.txt"
  then
      FAIL=1
      return 1
  fi

  echo "Test $TESTNO PASS"
  return 0
}

# Get a clean build of the project.
make clean
make

# Run against the test inputs.
if [ -x interpret ]; then
    testInterpreter 01 0
    testInterpreter 02 0
    testInterpreter 03 0
    testInterpreter 04 0
    testInterpreter 05 0
    testInterpreter 06 0
    testInterpreter 07 0
    testInterpreter 08 0
    testInterpreter 09 0
    testInterpreter 10 0
    testInterpreter 11 0
    testInterpreter 12 0
    testInterpreter 13 0
    testInterpreter 14 0
    testInterpreter 15 0
    testInterpreter 16 1
    testInterpreter 17 1
    testInterpreter 18 1
    testInterpreter 19 1
else
    fail "Since your program didn't compile, we couldn't test it"
fi

if [ $FAIL -ne 0 ]; then
  echo "FAILING TESTS!"
  exit 13
else 
  echo "Tests successful"
  exit 0
fi
