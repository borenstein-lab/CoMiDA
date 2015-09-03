#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem -rd ^ $TEST_DIR/data/input/testCustomReactionDelimiter.edges > testCustomReactionDelimiter.out
if [ $(diff testCustomReactionDelimiter.out $TEST_DIR/data/output/testCustomReactionDelimiter.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testCustomReactionDelimiter.out
