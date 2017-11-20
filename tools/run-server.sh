#!/bin/bash
GAME=$(./tools/get-shooter.sh)
$GAME -host 40000 | tee server.log

