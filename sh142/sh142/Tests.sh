#!/bin/sh

#  Tests.sh
#  sh142
#
#  Created by Jonathan.Guan on 11/19/11.
#  Copyright (c) 2011 __MyCompanyName__. All rights reserved.

# Tests
echo "Tests for Shell"

echo "ls -lA"
ls - lA

echo "pwd"
pwd


echo "Test piping"

echo "ps"
ps -lA | grep 0 | uniq


echo "ls"
ls ~jonathan.guan | sort | uniq | more

echo "Test && and ||"

echo "ls AND pwd OR ps"
ls && pwd || ps 

echo "ls OR..."
ls || abcdefg



echo "Test for loops"

echo "regular for loop"
for (n = 1; n <= 5; n++) echo $n forend

echo "for loop in AND exp"
ls && for (n = 1; n <= 5; n++) echo $n forend

echo "for loop in OR exp"
ls || for (n = 1; n <= 5; n++) echo $n forend

echo "for loop with pipe inside"
for (n = 0; n <= 5; n++) ps -lA | grep $n forend




