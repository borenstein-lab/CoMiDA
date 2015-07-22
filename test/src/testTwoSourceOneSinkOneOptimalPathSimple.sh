#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -s $TEST_DIR/data/input/testTwoSourceOneSinkOneOptimalPath.edges 01,02 05  > testTwoSourceOneSinkOneOptimalPath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoSourceOneSinkOneOptimalPath.out testTwoSourceOneSinkOneOptimalPath.species
rm testTwoSourceOneSinkOneOptimalPath.species
