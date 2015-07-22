#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testNoEdge.edges 01 02  > testNoEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testNoEdge.out testNoEdge.species
rm testNoEdge.species
