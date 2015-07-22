#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testIgnoreCycle.edges 01 02 > testIgnoreCycle.species
$BIN_DIR/check_output $TEST_DIR/data/output/testIgnoreCycle.out testIgnoreCycle.species
rm testIgnoreCycle.species
