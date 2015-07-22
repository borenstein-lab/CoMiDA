#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testTwoOptimalPaths.edges 01 02  > testTwoOptimalPaths.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoOptimalPaths.out testTwoOptimalPaths.species
rm testTwoOptimalPaths.species
