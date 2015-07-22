#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testLongerOptimalHyperPath.edges 01,02 05,06 > testLongerOptimalHyperPath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testLongerOptimalHyperPath.out testLongerOptimalHyperPath.species
rm testLongerOptimalHyperPath.species
