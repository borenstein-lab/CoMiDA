#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem -s $TEST_DIR/data/input/testOneSourceTwoSinksSharedEdgeSimple.edges > testOneSourceTwoSinksSharedEdgeSimple.out
if [ $(diff testOneSourceTwoSinksSharedEdgeSimple.out $TEST_DIR/data/output/testOneSourceTwoSinksSharedEdgeSimple.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testOneSourceTwoSinksSharedEdgeSimple.out
