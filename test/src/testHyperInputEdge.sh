#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperInputEdge.edges 01,02 03 > testHyperInputEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperInputEdge.out testHyperInputEdge.species
rm testHyperInputEdge.species
