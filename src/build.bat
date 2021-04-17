@echo off

mkdir "../build"
pushd "../build"

del "findorb11.exe"
clang -g -gcodeview -O2 -o "findorb11.exe" -D DEBUG ../src/"main.cpp"
