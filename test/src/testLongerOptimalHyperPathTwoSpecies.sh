#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testLongerOptimalHyperPathTwoSpecies.edges 01 03 > testLongerOptimalHyperPathTwoSpecies.out
if [ $(diff testLongerOptimalHyperPathTwoSpecies.out $TEST_DIR/data/output/testLongerOptimalHyperPathTwoSpecies.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testLongerOptimalHyperPathTwoSpecies.out
