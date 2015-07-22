#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperPartialCycle.edges 01 02 > testHyperPartialCycle.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperPartialCycle.out testHyperPartialCycle.species
rm testHyperPartialCycle.species
