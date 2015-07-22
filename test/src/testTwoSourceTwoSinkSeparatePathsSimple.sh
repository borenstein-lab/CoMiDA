#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testTwoSourceTwoSinkSeparatePaths.edges 01,02 03,04  > testTwoSourceTwoSinkSeparatePaths.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoSourceTwoSinkSeparatePaths.out testTwoSourceTwoSinkSeparatePaths.species
rm testTwoSourceTwoSinkSeparatePaths.species
