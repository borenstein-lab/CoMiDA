#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testLongerOptimalHyperPath.edges 01 03 > testLongerOptimalHyperPath.out
if [ $(diff testLongerOptimalHyperPath.out $TEST_DIR/data/output/testLongerOptimalHyperPath.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testLongerOptimalHyperPath.out
