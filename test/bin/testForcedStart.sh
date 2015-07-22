#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community -f 01 $TEST_DIR/data/input/testForcedStart.edges 01 02 > testForcedStart.species
$BIN_DIR/check_output $TEST_DIR/data/output/testForcedStart.out testForcedStart.species
rm testForcedStart.species
