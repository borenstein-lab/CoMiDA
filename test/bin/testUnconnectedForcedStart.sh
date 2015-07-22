#!/bin/bash
BIN_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/bin
TEST_DIR=/net/borenstein/vol1/PROJECTS/COMMUNITY_DESIGN_AE/community_design/test

$BIN_DIR/design_community -f 03 $TEST_DIR/data/input/testUnconnectedForcedStart.edges 01,03 02 > testUnconnectedForcedStart.species
$BIN_DIR/check_output $TEST_DIR/data/output/testUnconnectedForcedStart.out testUnconnectedForcedStart.species
rm testUnconnectedForcedStart.species
