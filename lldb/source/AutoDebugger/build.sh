export PATH=/local/mnt/workspace/pnirpal/prjs/llvm-project/build/bin:$PATH
export LIBRARY_PATH=/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib:$LD_LIBRARY_PATH

clang-format -i AutoDebugger.cpp

rm auto-lldb

clang++ -O0 -g AutoDebugger.cpp  -I/local/mnt/workspace/pnirpal/prjs/llvm-project/lldb/include -L/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib -llldb -o auto-lldb


./auto-lldb "/local/mnt/workspace/pnirpal/prjs/llvm-project/build/bin/opt -passes=vector-combine -mtriple=aarch64-unknown-linux-gnu neon.ll -S -o neon-post.ll" "/local/mnt/workspace/pnirpal/prjs/llvm-project/build/bin/opt -passes=vector-combine -mtriple=aarch64-unknown-linux-gnu sve.ll -S -o sve-post.ll" "/local/mnt/workspace/pnirpal/prjs/llvm-project/llvm/lib/Transforms/Vectorize/VectorCombine.cpp:4637" "/local/mnt/workspace/pnirpal/prjs/llvm-project/llvm/lib/Transforms/Vectorize/VectorCombine.cpp:4637"