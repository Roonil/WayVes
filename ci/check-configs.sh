#!/usr/bin/env bash
set -u

if [ $# -lt 1 ]; then
    echo "usage: $0 <WayVes binary> [config.yaml ...]" >&2
    exit 2
fi

BIN=$(realpath "$1")
shift
ROOT=$(cd "$(dirname "$0")/.." && pwd)
WAIT=${WAIT:-20}
ERROR_PATTERN='SHADER_COMPILATION_ERROR|PROGRAM_LINKING_ERROR|^Error:|Failed to open display|Gtk-CRITICAL|Gdk-CRITICAL'

isolate=()
if [ -z "${GITHUB_ACTIONS:-}" ]; then
    mkdir -p /tmp/WayVes
    if ! unshare -rm sh -c 'mount -t tmpfs tmpfs /tmp/WayVes' 2>/dev/null; then
        echo "Refusing to run: cannot give WayVes a private /tmp/WayVes, it would replace the live sockets" >&2
        exit 1
    fi
    isolate=(unshare -rm sh -c 'mount -t tmpfs tmpfs /tmp/WayVes && g=$1 && shift && exec unshare -U --map-user="$0" --map-group="$g" "$@"' "$(id -u)" "$(id -g)")
fi

if [ -d "$ROOT/shaders/utils" ]; then
    SHADER_ROOT="$ROOT/shaders"
else
    SHADER_ROOT="$ROOT"
fi

if [ $# -gt 0 ]; then
    CONFIGS=("$@")
else
    mapfile -t CONFIGS < <(
        find "$ROOT/starter-configs" "$ROOT/advanced-configs" -name '*.yaml' 2>/dev/null | sort | while read -r f; do
            grep -q '^audios:' "$f" && echo "${f#"$ROOT"/}"
        done
    )
fi

if [ ${#CONFIGS[@]} -eq 0 ]; then
    echo "No configs found" >&2
    exit 1
fi

failed=()

for cfg in "${CONFIGS[@]}"; do
    work=$(mktemp -d)
    prefix="$work/WayVes"
    mkdir -p "$prefix"

    for d in angular chain linear ncs utils; do
        ln -s "$SHADER_ROOT/$d" "$prefix/$d"
    done
    for e in "$ROOT/$(dirname "$cfg")"/*; do
        ln -sfn "$e" "$prefix/$(basename "$e")"
    done

    [ -n "${GITHUB_ACTIONS:-}" ] && echo "::group::$cfg"

    (cd "$prefix" && XDG_CONFIG_HOME="$work" timeout -k 5 -s INT "$WAIT" "${isolate[@]}" "$BIN" -i ci_check -f "$(basename "$cfg" .yaml)") >"$work/log" 2>&1
    rc=$?
    cat "$work/log"

    [ -n "${GITHUB_ACTIONS:-}" ] && echo "::endgroup::"

    reason=""
    if grep -qE "$ERROR_PATTERN" "$work/log"; then
        reason=$(grep -m1 -E "$ERROR_PATTERN" "$work/log")
    elif [ "$rc" -ne 124 ]; then
        reason="WayVes exited early with code $rc"
    fi

    if [ -n "$reason" ]; then
        echo "FAIL $cfg: $reason"
        [ -n "${GITHUB_ACTIONS:-}" ] && echo "::error file=$cfg,title=Config failed to compile::$reason"
        failed+=("$cfg")
    else
        echo "OK   $cfg"
    fi

    rm -rf "$work"
done

echo
echo "${#CONFIGS[@]} checked, ${#failed[@]} failed"
for f in "${failed[@]}"; do
    echo "  $f"
done

[ ${#failed[@]} -eq 0 ]
