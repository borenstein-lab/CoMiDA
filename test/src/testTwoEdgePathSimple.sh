#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testTwoEdgePath.edges 01 03  > testTwoEdgePath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoEdgePath.out testTwoEdgePath.species
rm testTwoEdgePath.species
