# -----------------------------------------------------------------------------
#  Contacts — developer Makefile (a thin, friendly wrapper over CMake)
#
#    make deps      install compilers, CMake, Ninja and Qt 6 for this OS
#    make run       build (Release) and launch the app
#    make test      build and run the unit tests
#    make help      list every target
#
#  Variables you can override:
#    BUILD_TYPE=Debug                    build flavour (default Release)
#    QT_PREFIX=/path/to/Qt/6.x/gcc_64    a Qt that CMake cannot find on its own
#    ARGS="--file /tmp/demo.dat"         arguments passed to the app by run/debug
# -----------------------------------------------------------------------------

APP_NAME   := qt-contacts
BUILD_TYPE ?= Release
BUILD_DIR  ?= build/$(BUILD_TYPE)
PREFIX     ?= $(HOME)/.local
ARGS       ?=

# ---- Platform detection -----------------------------------------------------
ifeq ($(OS),Windows_NT)
  PLATFORM := windows
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    PLATFORM := macos
  else
    PLATFORM := linux
  endif
endif

JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Prefer Ninja when it is installed — it is noticeably faster.
GENERATOR ?= $(if $(shell command -v ninja 2>/dev/null),Ninja,Unix Makefiles)

# Homebrew installs Qt outside CMake's default search path.
ifeq ($(PLATFORM),macos)
  QT_PREFIX ?= $(shell brew --prefix qt 2>/dev/null)
endif

CMAKE_FLAGS := -G "$(GENERATOR)" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
ifneq ($(strip $(QT_PREFIX)),)
  CMAKE_FLAGS += -DCMAKE_PREFIX_PATH="$(QT_PREFIX)"
endif

ifeq ($(PLATFORM),macos)
  APP_BIN := $(BUILD_DIR)/bin/$(APP_NAME).app/Contents/MacOS/$(APP_NAME)
else ifeq ($(PLATFORM),windows)
  APP_BIN := $(BUILD_DIR)/bin/$(APP_NAME).exe
else
  APP_BIN := $(BUILD_DIR)/bin/$(APP_NAME)
endif

SOURCES := $(shell find src tests -name '*.cpp' -o -name '*.h' 2>/dev/null)

# ---- Targets ----------------------------------------------------------------
.DEFAULT_GOAL := build
.PHONY: help deps configure build run test debug release rebuild clean distclean format format-check install

help: ## Show this help
	@echo "Contacts — make targets ($(PLATFORM), $(BUILD_TYPE)):"
	@grep -hE '^[a-zA-Z_-]+:.*## ' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*## "}; {printf "  \033[36m%-13s\033[0m %s\n", $$1, $$2}'

deps: ## Install the toolchain and Qt 6 for this OS
ifeq ($(PLATFORM),macos)
	@bash scripts/install-deps-macos.sh
else ifeq ($(PLATFORM),windows)
	@powershell -NoProfile -ExecutionPolicy Bypass -File scripts/install-deps-windows.ps1
else
	@bash scripts/install-deps-linux.sh
endif

configure: $(BUILD_DIR)/CMakeCache.txt ## Configure the CMake build directory

$(BUILD_DIR)/CMakeCache.txt:
	cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)

build: configure ## Build the app and the tests (default)
	cmake --build $(BUILD_DIR) --parallel $(JOBS)

run: build ## Build and launch the app
	"$(APP_BIN)" $(ARGS)

test: build ## Build and run the unit tests
	cd $(BUILD_DIR) && ctest --output-on-failure

debug: ## Build and launch a Debug build
	@$(MAKE) --no-print-directory run BUILD_TYPE=Debug

release: ## Build an optimized Release build
	@$(MAKE) --no-print-directory build BUILD_TYPE=Release

rebuild: clean build ## Clean, then build again

clean: ## Remove compiled objects (keeps the CMake configuration)
	@if [ -d "$(BUILD_DIR)" ]; then cmake --build $(BUILD_DIR) --target clean; fi

distclean: ## Delete every build directory
	rm -rf build

format: ## Format all sources with clang-format
	@command -v clang-format >/dev/null || { echo "clang-format is not installed"; exit 1; }
	clang-format -i $(SOURCES)

format-check: ## Fail if any source is not clang-formatted (for CI)
	@command -v clang-format >/dev/null || { echo "clang-format is not installed"; exit 1; }
	clang-format --dry-run --Werror $(SOURCES)

install: build ## Install the app under PREFIX (default ~/.local)
	cmake --install $(BUILD_DIR) --prefix "$(PREFIX)"
