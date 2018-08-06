#!/bin/bash
GAME=$(./tools/get-shooter.sh)
$GAME -norenderer -host -p 40000 | tee server.log

