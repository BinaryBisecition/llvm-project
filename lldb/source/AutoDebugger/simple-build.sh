clang-format -i simple.cpp

clang++ -O0 -g simple.cpp  -I/local/mnt/workspace/pnirpal/prjs/llvm-project/lldb/include -L/local/mnt/workspace/pnirpal/prjs/llvm-project/build/lib -llldb -o simple-lldb


./simple-lldb "/local/mnt/workspace/pnirpal/prjs/llvm-project/build/bin/opt -passes=vector-combine -mtriple=aarch64-unknown-linux-gnu neon.ll -S -o neon-post.ll" "/local/mnt/workspace/pnirpal/prjs/llvm-project/llvm/lib/Transforms/Vectorize/VectorCombine.cpp:4637"