#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperInputMissingSubstrate.edges 01 03 > testHyperInputMissingSubstrate.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperInputMissingSubstrate.out testHyperInputMissingSubstrate.species
rm testHyperInputMissingSubstrate.species
