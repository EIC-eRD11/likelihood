#!/bin/tcsh
source /opt/phenix/bin/phenix_setup.csh

echo "jobs " $1 "start"

set DIR=/data7/xueliang/eRD11/modular_rich_v1.0/modular_rich/likelihood/
set EXE=$DIR/bin/main
set INPUT=$DIR/input/datalist/$1.list
set OUTPUT=$DIR/output/$1.root

$EXE $INPUT $OUTPUT 


echo "jobs " $1 " done ;"
