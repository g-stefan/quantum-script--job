#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

echo "-> release quantum-script-extension-job"

cmdX(){
	if ! "$@" ; then
		echo "Error: release"
		exit 1
	fi
}

export XYO_PATH_RELEASE=release

cmdX /bin/sh ./build/ubuntu.sh make
cmdX /bin/sh ./build/ubuntu.sh install
cmdX /bin/sh ./build/ubuntu.sh install-release
cmdX xyo-cc quantum-script-extension-job --archive-release-sha512 --version-file=version.ini
