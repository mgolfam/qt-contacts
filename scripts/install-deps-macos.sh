#!/usr/bin/env bash
# Installs everything needed to build Contacts on macOS:
# Xcode Command Line Tools (clang, make), CMake, Ninja and Qt 6 via Homebrew.
set -euo pipefail

info() { printf '\033[1;36m==>\033[0m %s\n' "$*"; }
fail() { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

[[ "$(uname -s)" == "Darwin" ]] || fail "This script is for macOS."

if ! xcode-select -p >/dev/null 2>&1; then
    info "Installing the Xcode Command Line Tools (a system dialog will open)"
    xcode-select --install || true
    fail "Finish the Command Line Tools installation, then run this script again."
fi

if ! command -v brew >/dev/null; then
    fail "Homebrew is required. Install it from https://brew.sh with:
  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"
then run this script again."
fi

info "Updating Homebrew"
brew update

info "Installing CMake, Ninja and Qt 6"
brew install cmake ninja qt

QT_PREFIX="$(brew --prefix qt)"
info "Qt is installed at ${QT_PREFIX}"
info "The Makefile finds it automatically. For plain CMake use:"
echo "    cmake -S . -B build -DCMAKE_PREFIX_PATH=${QT_PREFIX}"

info "All set. Next step:  make run"
