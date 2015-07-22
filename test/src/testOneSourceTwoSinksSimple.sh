#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testOneSourceTwoSinks.edges 01 02,03  > testOneSourceTwoSinks.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinks.out testOneSourceTwoSinks.species
rm testOneSourceTwoSinks.species
