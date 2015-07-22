#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testOneSourceTwoSinksOneMissingPath.edges 01 02,03  > testOneSourceTwoSinksOneMissingPath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinksOneMissingPath.out testOneSourceTwoSinksOneMissingPath.species
rm testOneSourceTwoSinksOneMissingPath.species
