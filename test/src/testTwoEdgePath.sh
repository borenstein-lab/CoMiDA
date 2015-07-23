#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testTwoEdgePath.edges 01 03 > testTwoEdgePath.out
if [ $(diff testTwoEdgePath.out $TEST_DIR/data/output/testTwoEdgePath.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "ERROR: Observed output does not match the expected output."
else
	echo "Test Passed!"
fi
rm testTwoEdgePath.out
