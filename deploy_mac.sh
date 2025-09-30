#!/bin/bash
set -e

# Default build directory, can be overridden with first argument
BUILD_DIR="${1:-out/build/macos-release}"
APP_BUNDLE="$BUILD_DIR/Velocity.app"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ENTITLEMENTS="$SCRIPT_DIR/app.entitlements"

echo ">>> Starting macOS deployment..."
echo "*** Build directory: $BUILD_DIR"

# Qt discovery function
find_qt() {
    # Stage 1: Check if macdeployqt6 is already in PATH
    if command -v macdeployqt6 &> /dev/null; then
        echo "*** Found macdeployqt6 in PATH"
        return 0
    fi

    # Stage 2: Check environment variable (user explicit configuration)
    if [ -n "$QT6_PREFIX_PATH" ] && [ -x "$QT6_PREFIX_PATH/bin/macdeployqt6" ]; then
        echo "*** Found macdeployqt6 via QT6_PREFIX_PATH: $QT6_PREFIX_PATH/bin/macdeployqt6"
        export PATH="$QT6_PREFIX_PATH/bin:$PATH"
        return 0
    fi

    # Stage 3: Check common Qt installation locations (fallback)
    echo "*** macdeployqt6 not in PATH or QT6_PREFIX_PATH, searching common locations..."
    
    local qt_paths=(
        "$HOME/Qt/*/macos/bin"
        "/usr/local/opt/qt*/bin"
        "/opt/homebrew/opt/qt*/bin"
        "/Applications/Qt/*/macos/bin"
    )
    
    for pattern in "${qt_paths[@]}"; do
        for path in $pattern; do
            if [ -x "$path/macdeployqt6" ]; then
                echo "*** Found macdeployqt6 at: $path/macdeployqt6"
                export PATH="$path:$PATH"
                return 0
            fi
        done
    done

    return 1
}

# Find Qt using our discovery function
if ! find_qt; then
    echo ">>> ERROR: macdeployqt6 not found"
    echo "*** Please use one of these solutions:"
    echo "*** 1. Add Qt to your PATH: export PATH=\"\$HOME/Qt/[version]/macos/bin:\$PATH\""
    echo "*** 2. Set QT6_PREFIX_PATH: export QT6_PREFIX_PATH=\"/path/to/your/qt\""
    echo "*** 3. Install Qt via Homebrew: brew install qt"
    exit 1
fi

if ! command -v codesign &> /dev/null; then
    echo ">>> ERROR: codesign not found"
    echo "*** Please install Xcode command line tools:"
    echo "***   xcode-select --install"
    exit 1
fi

if [ ! -d "$APP_BUNDLE" ]; then
    echo ">>> ERROR: App bundle not found at $APP_BUNDLE"
    echo "*** Please build the project first:"
    echo "***   cmake --preset macos-release"
    echo "***   cmake --build --preset macos-release"
    exit 1
fi

if [ ! -f "$ENTITLEMENTS" ]; then
    echo ">>> ERROR: Entitlements file not found at $ENTITLEMENTS"
    echo "*** Please create app.entitlements file in project root"
    exit 1
fi

echo ">>> Deploying Qt frameworks..."
macdeployqt6 "$APP_BUNDLE" -always-overwrite

echo ">>> Code signing..."
codesign --entitlements "$ENTITLEMENTS" --deep --force --options runtime --sign "-" "$APP_BUNDLE"

echo ">>> SUCCESS: macOS deployment complete!"
echo "*** App bundle ready: $APP_BUNDLE"