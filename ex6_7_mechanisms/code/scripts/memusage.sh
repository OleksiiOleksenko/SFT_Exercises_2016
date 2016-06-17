#!/bin/bash

prog=$1

if [ -z "$prog" ];then
    echo "usage: $0 <program>"
    exit 1
fi

function get_pids {
    prog=$1
    pids=`ps ax | grep $prog | grep -v grep | cut -d ' ' -f 2`

    if [ -z "$pids" ];then
        echo "$prog is not running"
        exit 1
    fi
    echo $pids
}

pids=$(get_pids $prog)

echo "pid mem cpu prog par"
while [ ! -z "$pids" ]; do
    ps -eo pid,size,pcpu,args | grep $prog | grep -v grep | grep -v memusage.sh
    sleep 0.3
    pids=$(get_pids $prog)
done
