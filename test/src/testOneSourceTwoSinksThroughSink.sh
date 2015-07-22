#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testOneSourceTwoSinksThroughSink.edges 01 02,03  > testOneSourceTwoSinksThroughSink.species
$BIN_DIR/check_output $TEST_DIR/data/output/testOneSourceTwoSinksThroughSink.out testOneSourceTwoSinksThroughSink.species
rm testOneSourceTwoSinksThroughSink.species
