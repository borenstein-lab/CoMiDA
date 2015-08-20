#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem -spd @ $TEST_DIR/data/input/testCustomSubstrateProductDelimiter.edges > testCustomSubstrateProductDelimiter.out
if [ $(diff testCustomSubstrateProductDelimiter.out $TEST_DIR/data/output/testCustomSubstrateProductDelimiter.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testCustomSubstrateProductDelimiter.out
