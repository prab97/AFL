#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

# ---------------------------
# Docker wrapper
# ---------------------------
if [ -z "${IN_DOCKER:-}" ] && command -v docker >/dev/null 2>&1; then
    echo "[*] Docker detected — building and running container..."
    docker build -t js_test_image .
    docker run -it  -e  IN_DOCKER=1 -v "$(pwd)":/app -w /app js_test_image bash
    exit 0
fi

JS=js.c
PATCH=js.patch
ASAN_BIN=js_asan
FAST_BIN=js_fast
CRASH_DIR=in/crashes
CRASH_FILE="$CRASH_DIR/$(ls $CRASH_DIR | head -n1 || true)"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

echo "[*] Applying patch..."
patch -p0 < "$PATCH" && echo -e "${GREEN}Patch applied successfully ✅${NC}"

if [ -z "$CRASH_FILE" ]; then
    echo -e "${RED}[ERROR] No crash file found in $CRASH_DIR${NC}"
    exit 1
fi

echo "[*] Using crash file: $CRASH_FILE"
echo

echo "[*] Compiling ASan (diagnostic) binary..."
clang -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -o "$ASAN_BIN" "$JS"
echo "[+] ASan binary built: $ASAN_BIN"
echo

echo "[*] Running ASan binary on crash..."
./"$ASAN_BIN" "$CRASH_FILE" > result.txt  2>&1 || true

echo "=== ASan output (first 20 lines) ==="
head -n 20 result.txt
echo "=================================="
echo

if grep -q "AddressSanitizer\|Segmentation fault" result.txt; then
    echo -e "${RED}=== RESULT: FIX FAILED — crash still occurs ❌ ===${NC}"
else
    echo -e "${GREEN}=== RESULT: Crash no longer occurs — issue fixed ✅ ===${NC}"
fi

echo
echo "[*] Building fast instrumented binary for fuzzing..."
clang -O2 -g -o "$FAST_BIN" "$JS"
echo "[+] Fast binary built: $FAST_BIN"
