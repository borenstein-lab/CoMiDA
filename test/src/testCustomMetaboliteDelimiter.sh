#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem -md '(' $TEST_DIR/data/input/testCustomMetaboliteDelimiter.edges > testCustomMetaboliteDelimiter.out
if [ $(diff testCustomMetaboliteDelimiter.out $TEST_DIR/data/output/testCustomMetaboliteDelimiter.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testCustomMetaboliteDelimiter.out
