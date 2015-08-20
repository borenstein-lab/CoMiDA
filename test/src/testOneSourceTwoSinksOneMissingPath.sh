#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testOneSourceTwoSinksOneMissingPath.edges > testOneSourceTwoSinksOneMissingPath.out
if [ $(diff testOneSourceTwoSinksOneMissingPath.out $TEST_DIR/data/output/testOneSourceTwoSinksOneMissingPath.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testOneSourceTwoSinksOneMissingPath.out
