#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testLongerOptimalHyperPathTwoSpecies.edges 01,02 07,08 > testLongerOptimalHyperPathTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testLongerOptimalHyperPathTwoSpecies.out testLongerOptimalHyperPathTwoSpecies.species
rm testLongerOptimalHyperPathTwoSpecies.species
