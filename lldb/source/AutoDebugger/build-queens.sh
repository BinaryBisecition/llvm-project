export PATH=/local/mnt/workspace/pnirpal/prjs/llvm-project/build/bin:$PATH
export LIBRARY_PATH=/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib:$LD_LIBRARY_PATH

clang-format -i AutoDebugger.cpp

rm auto-lldb

clang++ -O0 -g AutoDebugger.cpp  -I/local/mnt/workspace/pnirpal/prjs/llvm-project/lldb/include -L/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib -llldb -o auto-lldb

clang++ -O0 -g Queens.cpp -o Queens
clang++ -O0 -g Queens-reg.cpp -o Queens-reg

# ./auto-lldb binary-baseline binary-exp baseline-bp exp-bp

./auto-lldb "Queens" "Queens-reg" "/local/mnt/workspace/pnirpal/prjs/llvm-project/lldb/source/AutoDebugger/Queens.cpp:528" "/local/mnt/workspace/pnirpal/prjs/llvm-project/lldb/source/AutoDebugger/Queens-reg.cpp:528"