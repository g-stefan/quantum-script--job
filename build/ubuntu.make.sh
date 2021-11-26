#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

ACTION=$1
if [ "$ACTION" = "" ]; then
	ACTION=make
fi

echo "- $BUILD_PROJECT > $ACTION"

cmdX(){
	if ! "$@" ; then
		echo "Error: $ACTION"
		exit 1
	fi
}

cmdX file-to-cs --touch=source/quantum-script-extension-job.cpp --file-in=source/quantum-script-extension-job.js --file-out=source/quantum-script-extension-job.src --is-string --name=extensionJobSource
cmdX xyo-cc --mode=$ACTION @build/source/quantum-script-extension-job.static.compile
cmdX xyo-cc --mode=$ACTION @build/source/quantum-script-extension-job.dynamic.compile
