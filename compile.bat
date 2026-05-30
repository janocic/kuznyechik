@echo off
echo Compiling...
g++ -std=c++11 -O2 -o test.exe test.cpp
g++ -std=c++11 -O2 -o unit_tests.exe unit_tests.cpp
echo Done!
pause

