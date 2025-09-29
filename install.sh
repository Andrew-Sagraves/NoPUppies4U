#!/bin/bash

set -e

EXEC_NAME="nopuppies4u"
BIN_DIR="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"
MAN_PAGE="nopuppies4u.1"

echo "Building project..."
make

echo "Installing executable to $BIN_DIR..."
sudo cp "$EXEC_NAME" "$BIN_DIR/"
sudo chmod 755 "$BIN_DIR/$EXEC_NAME"

echo "Installing man page to $MAN_DIR..."
sudo mkdir -p "$MAN_DIR"
sudo cp "man/$MAN_PAGE" "$MAN_DIR/"
sudo chmod 644 "$MAN_DIR/$MAN_PAGE"

echo "Updating man database..."
sudo mandb >/dev/null 2>&1 || true

echo "Installation complete!"
echo "You can now run '$EXEC_NAME' from anywhere, and see the man page with 'man $EXEC_NAME'."

