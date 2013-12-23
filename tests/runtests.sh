#!/usr/local/env bash

echo ""


echo "Creating test directories."

cd tests

mkdir test_directory
cd test_directory

touch level_one_file
ln -s level_one_file level_one_link
ln -s level_one_fake level_one_dead

mkdir directory
cd directory

touch level_two_file
ln -s level_two_file level_two_link
ln -s level_two_fake level_two_dead

cd ../..


echo ""
echo "Running without arguments:"
../bin/deadlink

echo ""
echo "Running without -r:"
../bin/deadlink -v test_directory

echo ""
echo "Running with -r:"
../bin/deadlink -rv test_directory


echo ""
echo "Removing test directories."
rm -rf test_directory


echo ""
