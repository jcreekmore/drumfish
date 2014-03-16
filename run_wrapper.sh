#!/bin/sh

cwd=$(dirname $(readlink -f $0))
prog=$(basename $0)
export LD_LIBRARY_PATH=${cwd}/../simavr/simavr/obj-$(gcc -dumpmachine)/
exec ${cwd}/.libs/${prog} "$@"
