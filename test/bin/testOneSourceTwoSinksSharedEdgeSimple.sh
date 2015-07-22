#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community -s $TEST_DIR/data/input/testOneSourceTwoSinksSharedEdge.edges 01 05,06  > testOneSourceTwoSinksSharedEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinksSharedEdge.out testOneSourceTwoSinksSharedEdge.species
rm testOneSourceTwoSinksSharedEdge.species
