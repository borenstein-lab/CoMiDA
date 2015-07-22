#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community -f 03 $TEST_DIR/data/input/testMissingForcedStart.edges 01,03 02 > testMissingForcedStart.species
$BIN_DIR/check_output $TEST_DIR/data/output/testMissingForcedStart.out testMissingForcedStart.species
rm testMissingForcedStart.species
