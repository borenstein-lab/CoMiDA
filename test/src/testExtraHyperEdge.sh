#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testExtraHyperEdge.edges 01,02 03 > testExtraHyperEdge.out
if [ $(diff testExtraHyperEdge.out $TEST_DIR/data/output/testExtraHyperEdge.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testExtraHyperEdge.out
