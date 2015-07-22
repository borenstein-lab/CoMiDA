#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperInputOutputEdge.edges 01,02 03,04 > testHyperInputOutputEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperInputOutputEdge.out testHyperInputOutputEdge.species
rm testHyperInputOutputEdge.species
