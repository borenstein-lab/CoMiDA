#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community $TEST_DIR/data/input/testOneSourceTwoSinks.edges 01 02,03  > testOneSourceTwoSinks.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinks.out testOneSourceTwoSinks.species
rm testOneSourceTwoSinks.species
