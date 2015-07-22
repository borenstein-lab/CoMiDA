#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoEdgePath.edges 01 03  > testTwoEdgePath.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoEdgePath.out testTwoEdgePath.species
rm testTwoEdgePath.species
