export COMPILER_PREFIXX= Actual ARM GNU Compiler
cmake -D CMAKE_BUILD_TYPE=Release ../



mkdir Build
cd Build
cmake ../
make



#-D CMAKE_CXX_COMPILER=g++-4.6