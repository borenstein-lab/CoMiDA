#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community -s $TEST_DIR/data/input/testOneSourceTwoSinksTwoSpecies.edges 01 02,03  > testOneSourceTwoSinksTwoSpecies.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinksTwoSpecies.out testOneSourceTwoSinksTwoSpecies.species
rm testOneSourceTwoSinksTwoSpecies.species
