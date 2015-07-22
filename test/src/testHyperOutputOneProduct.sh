#!/bin/bash
BIN_DIR=(BIN_DIR)
TEST_DIR=(TEST_DIR)

$BIN_DIR/design_community $TEST_DIR/data/input/testHyperOutputOneProduct.edges 01 02 > testHyperOutputOneProduct.species
$BIN_DIR/check_output $TEST_DIR/data/output/testHyperOutputOneProduct.out testHyperOutputOneProduct.species
rm testHyperOutputOneProduct.species
