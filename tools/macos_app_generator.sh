#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION_FILE="${ROOT_DIR}/VERSION"
APP_VERSION="0.1"
APP="${ROOT_DIR}/Music World Recreation.app"
BIN="${ROOT_DIR}/build/mw_recreation"
ICON_DIR="${ROOT_DIR}/assets/sprites/icons/apple"
ICON_FILE="${ICON_DIR}/icon.icns"
ICON_GENERATOR="${ROOT_DIR}/tools/generate_icons.py"
NO_BUILD=0
NO_ICON=0
NO_ICON_GEN=0

usage() {
  cat <<'USAGE'
Usage: ./tools/macos_app_generator.sh [--no-build] [--no-icon] [--no-icon-gen]

Options:
  --no-build   Skip CMake build step and only repackage .app
  --no-icon    Do not set app icon in the generated .app bundle
  --no-icon-gen  Do not run icon generation when apple icon is missing
  -h, --help   Show this help message
USAGE
}

for arg in "$@"; do
  case "$arg" in
    --no-build)
      NO_BUILD=1
      ;;
    --no-icon)
      NO_ICON=1
      ;;
    --no-icon-gen)
      NO_ICON_GEN=1
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "[ERROR] Unknown option: $arg"
      usage
      exit 1
      ;;
  esac
done

if [[ -f "${VERSION_FILE}" ]]; then
  RAW_VERSION="$(head -n 1 "${VERSION_FILE}" | tr -d '\r' | xargs)"
  if [[ -n "${RAW_VERSION}" ]]; then
    APP_VERSION="${RAW_VERSION}"
  fi
fi

if [[ "${NO_BUILD}" -eq 0 ]]; then
  # 1) Build binary
  cmake --fresh -S "${ROOT_DIR}" -B "${ROOT_DIR}/build" -DCMAKE_BUILD_TYPE=Release
  cmake --build "${ROOT_DIR}/build" -j
else
  echo "[INFO] --no-build is enabled: build step skipped."
fi

if [[ ! -x "${BIN}" ]]; then
  echo "[ERROR] Build output is missing: ${BIN}"
  echo "[ERROR] No build found, you need to build the project first."
  echo "[ERROR] Билда нет, нужно сбилдить!"
  exit 1
fi

ICON_AVAILABLE=0
ICON_PLIST_BLOCK=""
if [[ "${NO_ICON}" -eq 1 ]]; then
  echo "[INFO] --no-icon is enabled: icon setup skipped."
else
  if [[ ! -f "${ICON_FILE}" ]]; then
    if [[ "${NO_ICON_GEN}" -eq 1 ]]; then
      echo "[WARN] Apple icon is missing and --no-icon-gen is enabled: ${ICON_FILE}"
    else
      if [[ -f "${ICON_GENERATOR}" ]]; then
        if command -v python3 >/dev/null 2>&1; then
          echo "[INFO] Apple icon is missing, running icon generator..."
          if ! python3 "${ICON_GENERATOR}" --no-desktop --no-android; then
            echo "[WARN] Icon generator failed, icon will be skipped."
          fi
        else
          echo "[WARN] python3 is not found, icon generation skipped."
        fi
      else
        echo "[WARN] Icon generator script not found: ${ICON_GENERATOR}"
      fi
    fi
  fi

  if [[ -f "${ICON_FILE}" ]]; then
    ICON_AVAILABLE=1
    ICON_PLIST_BLOCK='  <key>CFBundleIconFile</key><string>icon.icns</string>'
    echo "[INFO] Apple icon found: ${ICON_FILE}"
  else
    echo "[WARN] Apple icon not found, app icon will not be set."
  fi
fi

# 2) Pack into .app
rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"

# Copy binary
cp "$BIN" "$APP/Contents/MacOS/mw_recreation.bin"

# Copy assets
cp -R -X "${ROOT_DIR}/assets" "$APP/Contents/Resources/assets"
ln -s ../Resources/assets "$APP/Contents/MacOS/assets"
if [[ "${ICON_AVAILABLE}" -eq 1 ]]; then
  cp "${ICON_FILE}" "$APP/Contents/Resources/icon.icns"
fi

# Launcher
cat > "$APP/Contents/MacOS/mw_recreation" <<'SH'
#!/bin/zsh
cd "$(dirname "$0")"
exec ./mw_recreation.bin "$@"
SH
chmod +x "$APP/Contents/MacOS/mw_recreation" "$APP/Contents/MacOS/mw_recreation.bin"

# Info.plist
cat > "$APP/Contents/Info.plist" <<PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key><string>Music World Recreation</string>
  <key>CFBundleDisplayName</key><string>Music World Recreation</string>
  <key>CFBundleIdentifier</key><string>com.ibworkshop.musicworldrecreation</string>
  <key>CFBundleVersion</key><string>${APP_VERSION}</string>
  <key>CFBundleShortVersionString</key><string>${APP_VERSION}</string>
  <key>CFBundleExecutable</key><string>mw_recreation</string>
  <key>CFBundlePackageType</key><string>APPL</string>
  <key>LSMinimumSystemVersion</key><string>10.15</string>
${ICON_PLIST_BLOCK}
</dict>
</plist>
PLIST

# Some files may carry macOS metadata (resource forks / xattrs) that break codesign.
xattr -cr "$APP"

# ad-hoc signature
codesign --force --deep --sign - "$APP"

# launch
if ! open "$APP"; then
  echo "[WARN] .app was created, but automatic launch via 'open' failed."
  echo "[WARN] You can run it manually:"
  echo "\"$APP/Contents/MacOS/mw_recreation\""
fi
