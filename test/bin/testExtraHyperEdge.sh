#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community $TEST_DIR/data/input/testExtraHyperEdge.edges 01,02 07,08 > testExtraHyperEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testExtraHyperEdge.out testExtraHyperEdge.species
rm testExtraHyperEdge.species
