#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testNoPath.edges 01 03  > testNoPath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testNoPath.out testNoPath.species
rm testNoPath.species
