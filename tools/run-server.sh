#!/bin/bash
GAME=$(./tools/get-shooter.sh)
$GAME -norenderer -host 40000 | tee server.log

