#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperOutputEdge.edges 01 02,03 > testHyperOutputEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperOutputEdge.out testHyperOutputEdge.species
rm testHyperOutputEdge.species
