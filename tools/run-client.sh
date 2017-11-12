#!/bin/bash
GAME=./bin/Shooter.app/Contents/MacOS/Shooter
$GAME -join 127.0.0.1 -p 40000 | tee client.log

