#!/usr/bin/env bash
GAME=$(./tools/get-shooter.sh)
$GAME -join 127.0.0.1 -p 40000 | tee client.log

