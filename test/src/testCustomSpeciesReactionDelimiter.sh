#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem -srd % $TEST_DIR/data/input/testCustomSpeciesReactionDelimiter.edges > testCustomSpeciesReactionDelimiter.out
if [ $(diff testCustomSpeciesReactionDelimiter.out $TEST_DIR/data/output/testCustomSpeciesReactionDelimiter.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testCustomSpeciesReactionDelimiter.out
