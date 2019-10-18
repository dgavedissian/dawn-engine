#!/usr/bin/env bash
if [ -f "./build/DwShooter${DEBUG_POSTFIX}" ]; then
	echo "./build/DwShooter${DEBUG_POSTFIX}"
elif [ -f "./cmake-build-debug/DwShooter${DEBUG_POSTFIX}" ]; then
	echo "./cmake-build-debug/DwShooter${DEBUG_POSTFIX}"
elif [ -f "./bin/Shooter.app/Contents/MacOS/Shooter" ]; then
	echo "./bin/Shooter.app/Contents/MacOS/Shooter"
else
	echo "UNKNOWN"
fi
