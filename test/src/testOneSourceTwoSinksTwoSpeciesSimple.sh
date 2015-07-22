#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testOneSourceTwoSinksTwoSpecies.edges 01 02,03  > testOneSourceTwoSinksTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinksTwoSpecies.out testOneSourceTwoSinksTwoSpecies.species
rm testOneSourceTwoSinksTwoSpecies.species
