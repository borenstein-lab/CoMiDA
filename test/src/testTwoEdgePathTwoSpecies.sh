#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoEdgePathTwoSpecies.edges 01 03  > testTwoEdgePathTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoEdgePathTwoSpecies.out testTwoEdgePathTwoSpecies.species
rm testTwoEdgePathTwoSpecies.species
