#!/bin/sh
# Install/enable S99webserver init script as a runlevel service (idempotent)
# Usage: sudo ./scripts/install_service.sh

set -eu

INIT_SCRIPT="/etc/init.d/S99webserver"
RC2_LINK="/etc/rc2.d/S99webserver"
RC5_DIR="/etc/rc5.d"
RC5_LINK="$RC5_DIR/S99webserver"

echo "== Install S99webserver service helper =="

if [ ! -f "$INIT_SCRIPT" ]; then
    echo "Error: init script not found at $INIT_SCRIPT"
    echo "Copy S99webserver to /etc/init.d/ first. Exiting."
    exit 1
fi

echo "Making sure init script is executable..."
chmod +x "$INIT_SCRIPT"

if [ -L "$RC2_LINK" ]; then
    echo "Removing old link: $RC2_LINK"
    rm -f "$RC2_LINK"
fi

echo "Ensuring runlevel directory exists: $RC5_DIR"
mkdir -p "$RC5_DIR"

if [ -L "$RC5_LINK" ]; then
    echo "Replacing existing link: $RC5_LINK"
    rm -f "$RC5_LINK"
fi

echo "Creating symlink: $RC5_LINK -> $INIT_SCRIPT"
ln -s "$INIT_SCRIPT" "$RC5_LINK"

echo "Listing created links and script permissions:"
ls -l "$RC5_LINK" || true
ls -l "$INIT_SCRIPT" || true

echo "Done. To apply, reboot the target system or run the script manually:"
echo "  sudo reboot"

echo "Notes:"
echo " - This helper creates /etc/rc5.d/S99webserver (S99 start script)."
echo " - On some minimal embedded systems runlevels differ; adjust RC dir if needed."
echo " - The init script assumes web files are served from /usr/web/ (as configured)."

exit 0
