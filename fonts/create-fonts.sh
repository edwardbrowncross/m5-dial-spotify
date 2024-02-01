#!/bin/bash

# https://fonts.google.com/specimen/Inter?query=inter

# ./fontconvert <Font-Name.ttf> <fontSize> <startGlyph> <endGlyph> [glyph whitelist] > ../src/assets/<filename>.h
./fontconvert Roboto-Regular.ttf 11 32 122 > ../src/assets/roboto_regular_11.h
./fontconvert Roboto-Bold.ttf 11 32 122  > ../src/assets/roboto_bold_11.h
./fontconvert Roboto-Medium.ttf 16 32 122 > ../src/assets/roboto_medium_16.h