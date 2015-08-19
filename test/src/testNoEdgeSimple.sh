#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testNoEdgeSimple.edges 01 02 > testNoEdgeSimple.out
if [ $(diff testNoEdgeSimple.out $TEST_DIR/data/output/testNoEdgeSimple.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testNoEdgeSimple.out
