#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/write_design_problem $TEST_DIR/data/input/testExtraEdgesSimple.edges 01 03 > testExtraEdgesSimple.out
