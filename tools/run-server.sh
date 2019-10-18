#!/usr/bin/env bash
GAME=$(./tools/get-shooter.sh)
$GAME -headless -host -p 40000 | tee server.log

