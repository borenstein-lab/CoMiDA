#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperTwoEdgePath.edges 01,02 05,06 > testHyperTwoEdgePath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperTwoEdgePath.out testHyperTwoEdgePath.species
rm testHyperTwoEdgePath.species
