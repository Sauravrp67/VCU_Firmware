#!/usr/bin/env bash
set -euo pipefail

config="${1:-release}"

case "${config}" in
  debug|release)
    ;;
  *)
    echo "usage: $0 [debug|release]" >&2
    exit 2
    ;;
esac

cmake --preset "${config}"
cmake --build --preset "${config}"
