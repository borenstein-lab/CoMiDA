#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community -fs 03 $TEST_DIR/data/input/testMissingForcedStart.edges 01,03 02 > testMissingForcedStart.species
$BIN_DIR/check_output $TEST_DIR/data/output/testMissingForcedStart.out testMissingForcedStart.species
rm testMissingForcedStart.species
