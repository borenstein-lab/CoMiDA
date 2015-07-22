#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testOneEdgePath.edges 01 02  > testOneEdgePath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneEdgePath.out testOneEdgePath.species
rm testOneEdgePath.species
