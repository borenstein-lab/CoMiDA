#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoLayersFullyConnectedInputs.edges 01,02 05,06 > testTwoLayersFullyConnectedInputs.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoLayersFullyConnectedInputs.out testTwoLayersFullyConnectedInputs.species
rm testTwoLayersFullyConnectedInputs.species
