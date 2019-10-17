#!/usr/bin/env bash
GAME=$(./tools/get-shooter.sh)
trap 'kill %1; kill %2' SIGINT
$GAME -norenderer -host 40000 | tee server.log | sed "s/^/[server] /" &
sleep 2
$GAME -join 127.0.0.1 -p 40000 | tee client.log | sed "s/^/[client] /"
wait

