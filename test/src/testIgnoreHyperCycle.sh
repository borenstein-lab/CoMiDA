#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testIgnoreHyperCycle.edges > testIgnoreHyperCycle.out
if [ $(diff testIgnoreHyperCycle.out $TEST_DIR/data/output/testIgnoreHyperCycle.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testIgnoreHyperCycle.out
