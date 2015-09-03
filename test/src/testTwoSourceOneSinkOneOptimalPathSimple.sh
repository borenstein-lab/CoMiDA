#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem -s $TEST_DIR/data/input/testTwoSourceOneSinkOneOptimalPathSimple.edges > testTwoSourceOneSinkOneOptimalPathSimple.out
if [ $(diff testTwoSourceOneSinkOneOptimalPathSimple.out $TEST_DIR/data/output/testTwoSourceOneSinkOneOptimalPathSimple.out | wc -l | awk '{print $1}') == 0 ]; then
	echo "Test Passed!"
else
	echo "ERROR: Observed output does not match the expected output."
fi
rm testTwoSourceOneSinkOneOptimalPathSimple.out
