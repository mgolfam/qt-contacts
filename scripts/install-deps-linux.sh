#!/usr/bin/env bash
# Installs everything needed to build Contacts on Linux:
# a C++17 compiler, CMake, Ninja, Git and Qt 6 (Core, Gui, Widgets, Test).
#
# Supported: Debian/Ubuntu (apt), Fedora/RHEL (dnf), Arch/Manjaro (pacman),
#            openSUSE (zypper).
set -euo pipefail

info() { printf '\033[1;36m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33mwarning:\033[0m %s\n' "$*" >&2; }
fail() { printf '\033[1;31merror:\033[0m %s\n' "$*" >&2; exit 1; }

SUDO=""
if [[ ${EUID} -ne 0 ]]; then
    command -v sudo >/dev/null || fail "Run this script as root, or install sudo."
    SUDO="sudo"
fi

if command -v apt-get >/dev/null; then
    info "Detected Debian/Ubuntu (apt)"
    ${SUDO} apt-get update
    if ! apt-cache show qt6-base-dev >/dev/null 2>&1; then
        fail "qt6-base-dev is not in your repositories (Ubuntu 22.04+ / Debian 12+ required).
       Install Qt 6 from https://www.qt.io/download-qt-installer and build with:
         make QT_PREFIX=\$HOME/Qt/6.x.x/gcc_64"
    fi
    ${SUDO} apt-get install -y build-essential cmake ninja-build git \
        qt6-base-dev qt6-base-dev-tools libgl1-mesa-dev
    # Native Wayland support (optional; X11 works without it).
    ${SUDO} apt-get install -y qt6-wayland || warn "qt6-wayland not available; X11/XWayland will be used."

elif command -v dnf >/dev/null; then
    info "Detected Fedora/RHEL (dnf)"
    ${SUDO} dnf install -y gcc-c++ make cmake ninja-build git qt6-qtbase-devel mesa-libGL-devel

elif command -v pacman >/dev/null; then
    info "Detected Arch Linux (pacman)"
    ${SUDO} pacman -Sy --needed --noconfirm base-devel cmake ninja git qt6-base

elif command -v zypper >/dev/null; then
    info "Detected openSUSE (zypper)"
    ${SUDO} zypper --non-interactive install gcc-c++ make cmake ninja git qt6-base-devel

else
    fail "Unsupported package manager. Install manually: a C++17 compiler, CMake ≥ 3.16, Ninja, Qt 6.2+ (Widgets, Test)."
fi

info "Installed versions:"
cmake --version | head -n1
ninja --version | sed 's/^/ninja /'
c++ --version | head -n1

info "All set. Next step:  make run"
