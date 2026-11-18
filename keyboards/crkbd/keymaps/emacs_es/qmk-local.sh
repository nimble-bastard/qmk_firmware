#!/bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
set -eu

usage() {
    cat <<'EOF'
Uso: sh qmk-local.sh [--plan|--resolve|--apply|--compile|--help]
  --plan     Muestra el plan sin crear archivos ni resolver dependencias.
  --resolve  Crea el entorno si falta y muestra qué instalaría pip.
  --apply    Instala QMK y los requirements del repo dentro del entorno.
  --compile  Compila crkbd/rev1:emacs_es sin instalar ni flashear.

QMK_VENV: ruta del entorno virtual (por defecto: .venv en la raíz del repo).
          Si es relativa, se interpreta desde la raíz del repo.
EOF
}

die() {
    printf '%s\n' "$*" >&2
    exit 1
}

mode=${1:---plan}
if [ "$#" -gt 1 ]; then
    usage >&2
    exit 2
fi
case $mode in
    --help|-h) usage; exit 0 ;;
    --plan|--resolve|--apply|--compile) ;;
    *) usage >&2; exit 2 ;;
esac

script_dir=$(CDPATH='' cd -P "$(dirname "$0")" && pwd)
repo=$(CDPATH='' cd -P "$script_dir/../../../.." && pwd)
cd "$repo"
[ -f requirements.txt ] && [ -f keyboards/crkbd/keymaps/emacs_es/keymap.c ] ||
    die 'No se encontró el repositorio QMK con el mapa emacs_es.'

venv=${QMK_VENV:-.venv}
case $venv in
    /*) ;;
    *) venv=$repo/$venv ;;
esac
python=$venv/bin/python

if [ "$mode" = --plan ]; then
    printf 'Repositorio: %s\nEntorno virtual: %s\n' "$repo" "$venv"
    printf '%s\n' \
        '--resolve: vista previa de pip para requirements.txt y qmk.' \
        '--apply: instalación de Python limitada al entorno virtual.' \
        '--compile: compilación de crkbd/rev1:emacs_es, sin flashear.'
    exit 0
fi

[ "$(id -u)" -ne 0 ] || die 'Ejecuta qmk-local.sh como usuario normal, sin sudo.'

case $mode in
    --resolve|--apply)
        if [ ! -d "$venv" ]; then
            python3 -m venv "$venv"
        fi
        [ -f "$venv/pyvenv.cfg" ] && [ -x "$python" ] ||
            die "No es un entorno virtual válido: $venv"
        set -- install -r requirements.txt qmk
        if [ "$mode" = --resolve ]; then
            set -- "$@" --dry-run --no-cache-dir
        fi
        "$python" -m pip --isolated --require-virtualenv "$@"
        ;;
    --compile)
        [ -f "$venv/pyvenv.cfg" ] && [ -x "$venv/bin/qmk" ] ||
            die 'Falta QMK en el entorno virtual. Ejecuta --apply primero.'
        command -v avr-gcc >/dev/null 2>&1 ||
            die 'Falta avr-gcc. Revisa el toolchain con gentoo-avr.sh.'
        [ -f lib/lufa/LUFA/Drivers/USB/USB.h ] ||
            die 'Falta LUFA. Ejecuta: git submodule update --init lib/lufa'
        PATH="$venv/bin:$PATH" SKIP_GIT=yes \
            "$venv/bin/qmk" compile -kb crkbd/rev1 -km emacs_es
        ;;
esac
