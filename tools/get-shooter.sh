#!/usr/bin/env bash
if [ -f "./bin/Shooter${DEBUG_POSTFIX}" ]; then
	echo "./bin/Shooter${DEBUG_POSTFIX}"
elif [ -f "./bin/Shooter.app/Contents/MacOS/Shooter" ]; then
	echo "./bin/Shooter.app/Contents/MacOS/Shooter"
else
	echo "UNKNOWN"
fi
