#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testOneSourceTwoSinksTwoSpecies.edges 01 03 > testOneSourceTwoSinksTwoSpecies.out
if [ $(diff testOneSourceTwoSinksTwoSpecies.out $TEST_DIR/data/output/testOneSourceTwoSinksTwoSpecies.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testOneSourceTwoSinksTwoSpecies.out
