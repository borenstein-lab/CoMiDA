#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -fs 03 $TEST_DIR/data/input/testUnconnectedForcedStart.edges 01,03 02 > testUnconnectedForcedStart.species
$BIN_DIR/check_output $TEST_DIR/data/output/testUnconnectedForcedStart.out testUnconnectedForcedStart.species
rm testUnconnectedForcedStart.species
