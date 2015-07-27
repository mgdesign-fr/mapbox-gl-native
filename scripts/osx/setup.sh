#!/usr/bin/env bash
# This script is sourced; do not set -e or -o pipefail here.

# Ensure mason is on the PATH
export PATH="`pwd`/.mason:${PATH}" MASON_DIR="`pwd`/.mason"

# Export Qt 4.7 paths
export PATH="/opt/qt-4.7/bin:${PATH}"
export PKG_CONFIG_PATH="/opt/qt-4.7/lib/pkgconfig"

# Set the core file limit to unlimited so a core file is generated upon crash
ulimit -c unlimited -S
