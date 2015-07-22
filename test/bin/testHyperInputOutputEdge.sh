#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperInputOutputEdge.edges 01,02 03,04 > testHyperInputOutputEdge.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperInputOutputEdge.out testHyperInputOutputEdge.species
rm testHyperInputOutputEdge.species
