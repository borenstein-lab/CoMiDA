#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testTwoOptimalPathsSimple.edges 01 03 > testTwoOptimalPathsSimple.out
if [ $(diff testTwoOptimalPathsSimple.out $TEST_DIR/data/output/testTwoOptimalPathsSimple.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testTwoOptimalPathsSimple.out
