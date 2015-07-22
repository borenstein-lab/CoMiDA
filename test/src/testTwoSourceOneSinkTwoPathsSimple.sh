#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testTwoSourceOneSinkTwoPaths.edges 01,02 03  > testTwoSourceOneSinkTwoPaths.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoSourceOneSinkTwoPaths.out testTwoSourceOneSinkTwoPaths.species
rm testTwoSourceOneSinkTwoPaths.species
