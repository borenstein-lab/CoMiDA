#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testLongerOptimalPath.edges 01 03  > testLongerOptimalPath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testLongerOptimalPath.out testLongerOptimalPath.species
rm testLongerOptimalPath.species
