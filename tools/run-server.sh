#!/bin/bash
GAME=./bin/Shooter.app/Contents/MacOS/Shooter
$GAME -host 40000 | tee server.log

