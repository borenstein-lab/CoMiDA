#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoProductsTwoHyperPathsSharedSubstrate.edges 01,02,04 03,05 > testTwoProductsTwoHyperPathsSharedSubstrate.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoProductsTwoHyperPathsSharedSubstrate.out testTwoProductsTwoHyperPathsSharedSubstrate.species
rm testTwoProductsTwoHyperPathsSharedSubstrate.species
