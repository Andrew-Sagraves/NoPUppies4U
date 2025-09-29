#!/bin/bash
set -e

EXEC_NAME="nopuppies4u"
BIN_DIR="/usr/local/bin"
MAN_DIR="/usr/local/share/man/man1"
MAN_PAGE="nopuppies4u.1"

echo "Uninstalling $EXEC_NAME..."

if [ -f "$BIN_DIR/$EXEC_NAME" ]; then
    echo "Removing $BIN_DIR/$EXEC_NAME"
    sudo rm -f "$BIN_DIR/$EXEC_NAME"
else
    echo "Executable not found in $BIN_DIR"
fi

if [ -f "$MAN_DIR/$MAN_PAGE" ]; then
    echo "Removing $MAN_DIR/$MAN_PAGE"
    sudo rm -f "$MAN_DIR/$MAN_PAGE"
else
    echo "Man page not found in $MAN_DIR"
fi

echo "Updating man database..."
sudo mandb > /dev/null

echo "Uninstall complete!"
