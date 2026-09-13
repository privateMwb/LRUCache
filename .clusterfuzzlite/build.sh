#!/bin/bash -eu
# ============================================================
# .clusterfuzzlite/build.sh
#
# LRUCachePro is header-only, so unlike a harness that needs to compile
# separate .cpp translation units first, this just compiles the fuzz
# target directly against the headers under include/.
#
# Add more `${SRC}/LRUCachePro/fuzz/fuzz_*.cpp` harnesses here as
# they're added; each becomes its own $OUT binary.
# ============================================================

cd "${SRC}/LRUCachePro"

$CXX $CXXFLAGS -std=c++20 \
  -I"${SRC}/LRUCachePro/include" \
  fuzz/fuzz_lrucache.cpp \
  $LIB_FUZZING_ENGINE \
  -o "${OUT}/fuzz_lrucache"
