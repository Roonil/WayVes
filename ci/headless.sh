#!/usr/bin/env bash
set -u

export XDG_RUNTIME_DIR=$(mktemp -d)
chmod 700 "$XDG_RUNTIME_DIR"

export WLR_BACKENDS=headless
export WLR_RENDERER=pixman
export WLR_LIBINPUT_NO_DEVICES=1
export WLR_HEADLESS_OUTPUTS=1
export LIBGL_ALWAYS_SOFTWARE=1
export GDK_BACKEND=wayland
export GTK_A11Y=none
export NO_AT_BRIDGE=1
unset DISPLAY WAYLAND_DISPLAY

trap 'kill $(jobs -p) 2>/dev/null' EXIT

pipewire >/dev/null 2>&1 &
wireplumber >/dev/null 2>&1 &

echo 'output HEADLESS-1 resolution 2560x1600' >"$XDG_RUNTIME_DIR/sway.conf"
sway -c "$XDG_RUNTIME_DIR/sway.conf" >"$XDG_RUNTIME_DIR/sway.log" 2>&1 &

for _ in $(seq 50); do
    socket=$(find "$XDG_RUNTIME_DIR" -maxdepth 1 -name 'wayland-*' ! -name '*.lock' | head -1)
    [ -n "$socket" ] && break
    sleep 0.2
done

if [ -z "${socket:-}" ]; then
    echo "sway did not start:" >&2
    cat "$XDG_RUNTIME_DIR/sway.log" >&2
    exit 1
fi

export WAYLAND_DISPLAY=$(basename "$socket")

"$(dirname "$0")/check-configs.sh" "$@"
