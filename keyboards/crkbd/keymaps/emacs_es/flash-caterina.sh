#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
set -eu

usage() {
    cat <<'EOF'
Uso: sh flash-caterina.sh [--plan|--apply|--help]
  --plan   Muestra el firmware y el bootloader esperado; no escribe nada.
  --apply  Espera hasta 30 segundos y flashea una sola mitad por USB.

Desconecta el USB antes de separar o unir las mitades por TRRS.
Conecta solo la mitad que vas a flashear y pulsa RESET dos veces rápido
cuando aparezca el mensaje de espera.
EOF
}

die() {
    printf '%s\n' "$*" >&2
    exit 1
}

is_caterina() (
    # Usa los IDs del primer dispositivo USB padre del puerto serie.
    CDPATH='' cd -P "$1/device" || exit 1
    while [ "$PWD" != / ] && [ "$PWD" != /sys ]; do
        if [ -r idVendor ] && [ -r idProduct ]; then
            IFS= read -r vid < idVendor && IFS= read -r pid < idProduct &&
                [ "$vid:$pid" = 1b4f:9205 ]
            exit "$?"
        fi
        cd -P .. || exit 1
    done
    exit 1
)

mode=${1:---plan}
if [ "$#" -gt 1 ]; then
    usage >&2
    exit 2
fi
case $mode in
    --help|-h) usage; exit 0 ;;
    --plan|--apply) ;;
    *) usage >&2; exit 2 ;;
esac

script_dir=$(CDPATH='' cd -P "$(dirname "$0")" && pwd)
repo=$(CDPATH='' cd -P "$script_dir/../../../.." && pwd)
firmware=$repo/crkbd_rev1_emacs_es.hex

printf 'Firmware: %s\nBootloader: Caterina (USB 1b4f:9205)\n' "$firmware"
if [ "$mode" = --plan ]; then
    if [ ! -s "$firmware" ]; then
        printf '%s\n' 'Todavía falta el archivo HEX; compila con qmk-local.sh --compile.'
    fi
    printf '%s\n' 'No se escribió nada. Usa --apply para flashear una mitad.'
    exit 0
fi

[ -f "$firmware" ] && [ -s "$firmware" ] && [ -r "$firmware" ] ||
    die 'Falta un archivo HEX legible. Ejecuta qmk-local.sh --compile primero.'
command -v avrdude >/dev/null 2>&1 ||
    die 'Falta avrdude. En Gentoo: sudo emerge --ask --verbose dev-embedded/avrdude'
[ -d /sys/class/tty ] || die 'No se encontró sysfs. Ejecuta el script en Linux.'

printf '%s\n' 'Esperando Caterina (30 s). Pulsa RESET dos veces rápido en la mitad conectada.'
remaining=30
while :; do
    port=
    for tty in /sys/class/tty/ttyACM*; do
        [ -d "$tty/device" ] || continue
        is_caterina "$tty" 2>/dev/null || continue
        [ -z "$port" ] || die 'Hay más de un Caterina conectado. Deja conectada una sola mitad.'
        port=/dev/${tty##*/}
    done
    if [ -n "$port" ] && [ -c "$port" ]; then
        [ -r "$port" ] && [ -w "$port" ] ||
            die "Sin permisos de lectura y escritura en $port. Revisa los permisos del puerto en Gentoo."
        printf 'Flasheando y verificando en %s…\n' "$port"
        exec avrdude -p atmega32u4 -c avr109 -P "$port" -U "flash:w:$firmware:i"
    fi
    [ "$remaining" -gt 0 ] ||
        die 'No apareció el puerto de Caterina. Conecta la mitad directamente por USB y vuelve a intentar.'
    sleep 1
    remaining=$((remaining - 1))
done
