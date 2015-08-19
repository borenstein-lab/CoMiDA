#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testHyperPartialCycle.edges 01 02 > testHyperPartialCycle.out
if [ $(diff testHyperPartialCycle.out $TEST_DIR/data/output/testHyperPartialCycle.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testHyperPartialCycle.out
