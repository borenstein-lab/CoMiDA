#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testOneEdgePathSimple.edges 01 03 > testOneEdgePathSimple.out
if [ $(diff testOneEdgePathSimple.out $TEST_DIR/data/output/testOneEdgePathSimple.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "ERROR: Observed output does not match the expected output."
else
	echo "Test Passed!"
fi
rm testOneEdgePathSimple.out
