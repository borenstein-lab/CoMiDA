#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testOneSourceTwoSinksSharedEdge.edges 01 05,06  > testOneSourceTwoSinksSharedEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinksSharedEdge.out testOneSourceTwoSinksSharedEdge.species
rm testOneSourceTwoSinksSharedEdge.species
