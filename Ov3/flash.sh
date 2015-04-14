#!/bin/bash

if [ -z $1 ];then
	echo "No package specified"
	exit 1
fi
 
ptxdist compile $1 || exit 1
ptxdist targetinstall $1 || exit 1
ptxdist image root.romfs || exit 1
ptxdist test flash-rootfs && echo "Successfully updated package $1"
