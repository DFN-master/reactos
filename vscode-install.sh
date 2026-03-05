#!/bin/bash
# VS Code Installer for ReactOS (Linux/VM variant)
# Use this script when running ReactOS in a Linux/WSL environment

set -e

echo "========================================================================"
echo "VS Code Installation for ReactOS - Linux/VM Variant"
echo "========================================================================"
echo ""

# Configuration
VSCODE_VERSION="1.60.2"
VSCODE_URL="https://github.com/microsoft/vscode/releases/download/1.60.2/VSCode-win32-x64-1.60.2.zip"
DOWNLOAD_DIR="$HOME/Downloads"
VSCODE_DIR="/mnt/c/VSCode-Portable"  # Adjust path for your setup

echo "[Step 1] Checking prerequisites..."
echo ""

# Check for curl or wget
if ! command -v curl &> /dev/null && ! command -v wget &> /dev/null; then
    echo "ERROR: curl or wget not found. Please install one of them."
    exit 1
fi

# Check for unzip
if ! command -v unzip &> /dev/null; then
    echo "ERROR: unzip not found. Please install it."
    exit 1
fi

echo "[Step 2] Downloading VS Code $VSCODE_VERSION..."
echo ""

mkdir -p "$DOWNLOAD_DIR"

if [ -f "$DOWNLOAD_DIR/VSCode-win32-x64-$VSCODE_VERSION.zip" ]; then
    echo "   [✓] VS Code already downloaded"
else
    if command -v curl &> /dev/null; then
        curl -L -o "$DOWNLOAD_DIR/VSCode-win32-x64-$VSCODE_VERSION.zip" "$VSCODE_URL"
    else
        wget -O "$DOWNLOAD_DIR/VSCode-win32-x64-$VSCODE_VERSION.zip" "$VSCODE_URL"
    fi
    echo "   [✓] Download completed"
fi

echo ""
echo "[Step 3] Extracting VS Code..."
echo ""

mkdir -p "$VSCODE_DIR"
unzip -o "$DOWNLOAD_DIR/VSCode-win32-x64-$VSCODE_VERSION.zip" -d "$VSCODE_DIR"
echo "   [✓] VS Code extracted"

echo ""
echo "[Step 4] Creating launch script..."
echo ""

cat > "$VSCODE_DIR/launch-vscode.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./Code.exe \
  --disable-gpu \
  --disable-hardware-acceleration \
  --no-sandbox \
  --disable-dev-shm-usage \
  --disable-telemetry \
  "$@"
EOF

chmod +x "$VSCODE_DIR/launch-vscode.sh"
echo "   [✓] Launch script created"

echo ""
echo "========================================================================"
echo "Installation Complete!"
echo "========================================================================"
echo ""
echo "Run VS Code:"
echo "   $VSCODE_DIR/Code.exe --disable-gpu --no-sandbox"
echo ""
echo "Or use the launch script:"
echo "   $VSCODE_DIR/launch-vscode.sh [file_or_folder]"
echo ""
