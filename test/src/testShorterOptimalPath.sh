#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testShorterOptimalPath.edges 01 02  > testShorterOptimalPath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testShorterOptimalPath.out testShorterOptimalPath.species
rm testShorterOptimalPath.species
