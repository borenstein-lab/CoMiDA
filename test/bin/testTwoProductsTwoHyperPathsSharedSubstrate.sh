#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community $TEST_DIR/data/input/testTwoProductsTwoHyperPathsSharedSubstrate.edges 01,02,04 03,05 > testTwoProductsTwoHyperPathsSharedSubstrate.species
$BIN_DIR/check_output $TEST_DIR/data/output/testTwoProductsTwoHyperPathsSharedSubstrate.out testTwoProductsTwoHyperPathsSharedSubstrate.species
rm testTwoProductsTwoHyperPathsSharedSubstrate.species
