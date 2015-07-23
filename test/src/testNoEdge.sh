#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testNoEdge.edges 01 03 > testNoEdge.out
if [ $(diff testNoEdge.out $TEST_DIR/data/output/testNoEdge.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testNoEdge.out
