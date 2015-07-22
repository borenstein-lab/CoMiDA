#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoSourceTwoSinkSharedIntermediate.edges 01,02 04,05  > testTwoSourceTwoSinkSharedIntermediate.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoSourceTwoSinkSharedIntermediate.out testTwoSourceTwoSinkSharedIntermediate.species
rm testTwoSourceTwoSinkSharedIntermediate.species
