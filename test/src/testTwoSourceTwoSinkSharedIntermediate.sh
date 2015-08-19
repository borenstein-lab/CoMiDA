#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testTwoSourceTwoSinkSharedIntermediate.edges 01,02 04,05 > testTwoSourceTwoSinkSharedIntermediate.out
if [ $(diff testTwoSourceTwoSinkSharedIntermediate.out $TEST_DIR/data/output/testTwoSourceTwoSinkSharedIntermediate.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testTwoSourceTwoSinkSharedIntermediate.out
