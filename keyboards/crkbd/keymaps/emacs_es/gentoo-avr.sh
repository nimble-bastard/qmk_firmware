#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
set -eu

usage() {
    cat <<'EOF'
Uso: sh gentoo-avr.sh [--plan|--apply|--help]
  --plan   Comprueba el toolchain y muestra el plan (opción por defecto).
  --apply  Instala con Portage/crossdev si no hay un toolchain AVR funcional.

QMK_AVR_OVERLAY: ruta absoluta del overlay dedicado
                 (por defecto: /var/db/repos/crossdev).
EOF
}

have_avr() {
    command -v avr-gcc >/dev/null 2>&1 &&
        command -v avr-objcopy >/dev/null 2>&1 &&
        printf '#include <avr/io.h>\n' |
        avr-gcc -mmcu=atmega32u4 -E -x c - >/dev/null 2>&1 &&
        avr_libc=$(avr-gcc -mmcu=atmega32u4 -print-file-name=libc.a) &&
        [ "$avr_libc" != libc.a ] && [ -r "$avr_libc" ]
}

as_root() {
    if [ "$(id -u)" -eq 0 ]; then
        "$@"
    else
        sudo "$@"
    fi
}

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

if have_avr; then
    printf '%s\n' 'Toolchain para ATmega32U4 disponible. No se necesita instalar nada.'
    exit 0
fi

overlay=${QMK_AVR_OVERLAY:-/var/db/repos/crossdev}
case $overlay in
    /*) ;;
    *) printf '%s\n' 'QMK_AVR_OVERLAY debe ser una ruta absoluta.' >&2; exit 2 ;;
esac

if ! command -v emerge >/dev/null 2>&1; then
    printf '%s\n' 'No se encontró emerge. Ejecuta este script en Gentoo.' >&2
    exit 1
fi

printf 'Objetivo: avr (ATmega32U4)\nOverlay: %s\n' "$overlay"
if [ "$mode" = --plan ]; then
    if command -v crossdev >/dev/null 2>&1; then
        crossdev --target avr --show-target-cfg
    else
        emerge --pretend --verbose sys-devel/crossdev
    fi
    printf '%s\n' 'Con --apply, crossdev construirá binutils, GCC y avr-libc para AVR.'
    exit 0
fi

if ! command -v crossdev >/dev/null 2>&1; then
    as_root emerge --ask --verbose sys-devel/crossdev
fi
as_root crossdev --stable --target avr --ov-output "$overlay" --portage '--ask --verbose'
if ! have_avr; then
    printf '%s\n' 'El toolchain AVR sigue incompleto o no está en PATH.' >&2
    exit 1
fi
avr-gcc --version
