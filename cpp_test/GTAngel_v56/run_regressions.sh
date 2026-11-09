#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
node generate_v27.mjs
node verify_v27_source.mjs
mkdir -p .validation
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
cp ./*.hpp ./*.cpp "$tmp/"
cp gtangel_agi_v27_cns.hpp "$tmp/gtangel_agi_v20_cns.hpp"
cp -r assets "$tmp/"
for name in verify_v19_gameplay verify_v20_cns verify_v23_cns verify_v24_display verify_v24_renderer verify_v25_commons verify_v25_renderer verify_v26_bridge; do
    echo "TEST $name against v27 CNS"
    (cd "$tmp" && g++ -std=c++17 -O2 "$name.cpp" -o "$name" && "./$name") >".validation/$name.log" 2>&1
    tail -2 ".validation/$name.log"
done
for name in verify_v27_learning verify_v27_authority; do
    g++ -std=c++17 -O2 "$name.cpp" -o "$name"
    "./$name" | tee ".validation/$name.log"
done
