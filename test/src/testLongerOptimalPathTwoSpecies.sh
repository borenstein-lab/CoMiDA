#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testLongerOptimalPathTwoSpecies.edges 01 04  > testLongerOptimalPathTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testLongerOptimalPathTwoSpecies.out testLongerOptimalPathTwoSpecies.species
rm testLongerOptimalPathTwoSpecies.species
