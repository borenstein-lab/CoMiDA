#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoHyperEdgePathTwoSpecies.edges 01,02 05,06 > testTwoHyperEdgePathTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoHyperEdgePathTwoSpecies.out testTwoHyperEdgePathTwoSpecies.species
rm testTwoHyperEdgePathTwoSpecies.species
