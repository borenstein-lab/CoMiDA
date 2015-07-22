#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testExtraEdges.edges 01 03 > testExtraEdges.species
$BIN_DIR/check_output $TEST_DIR/data/output/testExtraEdges.out testExtraEdges.species
rm testExtraEdges.species
