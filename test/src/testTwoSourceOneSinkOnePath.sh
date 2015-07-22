#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoSourceOneSinkOnePath.edges 01,02 03  > testTwoSourceOneSinkOnePath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoSourceOneSinkOnePath.out testTwoSourceOneSinkOnePath.species
rm testTwoSourceOneSinkOnePath.species
