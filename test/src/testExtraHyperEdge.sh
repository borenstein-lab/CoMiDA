#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testExtraHyperEdge.edges 01,02 07,08 > testExtraHyperEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testExtraHyperEdge.out testExtraHyperEdge.species
rm testExtraHyperEdge.species
