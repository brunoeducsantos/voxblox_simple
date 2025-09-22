#!/usr/bin/env bash
set -e

JOBS=$(nproc)

# ==========================
# Build Protobuf
# ==========================
if [ -d thirdparty/protobuf ]; then
  echo "🔨 Building Protobuf..."
  cd thirdparty/protobuf
  git submodule update --init --recursive
  mkdir -p build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF
  make -j${JOBS}
  sudo make install
  sudo ldconfig
  cd ../../..
else
  echo "❌ thirdparty/protobuf not found"
fi

# ==========================
# Build Glog
# ==========================
if [ -d thirdparty/glog ]; then
  echo "🔨 Building Glog..."
  cd thirdparty/glog
  mkdir -p build && cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release
  make -j${JOBS}
  sudo make install
  sudo ldconfig
  cd ../../..
else
  echo "❌ thirdparty/glog not found"
fi

echo "✅ Protobuf and Glog built and installed!"

