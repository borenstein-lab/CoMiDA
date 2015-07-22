#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community $TEST_DIR/data/input/testLongerOptimalHyperPathTwoSpecies.edges 01,02 07,08 > testLongerOptimalHyperPathTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testLongerOptimalHyperPathTwoSpecies.out testLongerOptimalHyperPathTwoSpecies.species
rm testLongerOptimalHyperPathTwoSpecies.species
