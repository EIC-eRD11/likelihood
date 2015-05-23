#!/bin/csh
echo " welcome to process.csh ! "

set DIR = /data7/xueliang/eRD11/modular_rich/likelihood/condor 
set EXE = $DIR/run.csh

$EXE $1 
echo "job $1 finished"
