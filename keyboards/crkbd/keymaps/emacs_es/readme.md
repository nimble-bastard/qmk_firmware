# Corne de 42 teclas para Emacs y vi

Este mapa está pensado para programar con Emacs clásico y usar `vi` por SSH,
sin cambiar el mapa al pasar el mismo teclado de Linux a Windows con un switch
USB. Los ajustes del Corne viven en el firmware; Windows no necesita QMK.

Usa la distribución **US normal** en Linux y Windows. Toca `Tab/SÍM` para
enviar Tab o `Ent/NAV` para enviar Enter. Mantén el pulgar pulsado para
activar su capa momentánea.

## BASE — QWERTY US

```text
Esc    Q      W      E      R        T      │ Y      U        I      O      P      Bksp
Shift  A      S      D      F        G      │ H      J        K      L      ;      Shift
Ctrl   Z      X      C      V        B      │ N      M        ,      .      /      '
                     Super  Tab/SÍM  Esp    │ Esp    Ent/NAV  Meta
```

La última fila muestra los pulgares en orden físico, de izquierda a derecha.
Los dos pulgares interiores envían Espacio. `Ctrl` y `Meta` son teclas
dedicadas. `Meta` envía Alt izquierdo.

**Super** está en el pulgar exterior izquierdo y envía `KC_LGUI` (Super en
Linux, tecla Windows en Windows). Ambos `Shift` están en los extremos de
la fila central. El apóstrofo queda en la esquina inferior derecha.
En BASE, cualquiera de los dos Shift con Backspace envía Suprimir.

## SÍM — símbolos para programar

```text
Enter  ~      !      @      #      $      │ %      ^      &      *      \      |
Super  (      )      [      ]      ·      │ `      {      }      <      >      ·
·      -      _      =      +      ·      │ ·      :      ?      ·      ·      "
                     ·      ·      ·      │ ·      ·      ·
```

`SÍM + Esc` envía Enter y `SÍM + Shift izquierdo` envía
Super. `SÍM + H` escribe el backtick (`` ` ``) y `SÍM + '` escribe
comillas dobles. El Shift derecho conserva su función.

## NAV — números y navegación

```text
·      7      8      9      ·      Win    │ ·      Inicio ↑      Fin    RePág  ·
·      4      5      6      ·      ·      │ ·      ←      ↓      →      AvPág  Meta
·      1      2      3      0      Linux  │ ·      ·      ·      ·      ·      Caps
                     ·      ·      ·      │ ·      ·      ·
```

`·` indica una tecla transparente, no una tecla desactivada: si solo está
activa esa capa, conserva su función de la base.

- **Enter:** un toque en `Ent/NAV` envía Enter; no hay una tecla Enter
  adicional dentro de la capa NAV.
- **Backspace y Meta:** `NAV + Backspace` sigue siendo Backspace sin Shift.
  `NAV + Shift derecho` envía Meta (Alt izquierdo).
- **Bloq Mayús:** `NAV + '` alterna Bloq Mayús (`Caps`); sin NAV, la tecla
  escribe el apóstrofo.
- **Flechas en T invertida:** `I` es `↑`; `J`, `K` y `L` son `←`, `↓` y
  `→`. En `vi`, puedes seguir usando `hjkl` sin capa.
- **Inicio y Fin:** `NAV + U` y `NAV + O`, respectivamente.
- **Números:** siguen agrupados en la mitad izquierda.

**Atajos de Sway (`$mod` = `Mod4`):** para `Super+1`, mantén `Super` con
el pulgar izquierdo y `NAV` con el derecho, y pulsa `Z` (`Z` da `1` en
NAV). Para `Super+Shift+Q`, no actives ninguna capa.

## Gestos de puntuación

| Mantén pulsado | Toca | Un toque | Dos toques rápidos |
| --- | --- | --- | --- |
| Shift izquierdo | Shift derecho | `?` | `¿` |
| Shift derecho | Shift izquierdo | `!` | `¡` |

El signo sale al soltar el Shift que mantuviste pulsado. Cada toque y la
pausa entre dos toques deben durar menos de 200 ms. Si pulsas otra tecla,
el gesto se cancela y los Shift siguen actuando como modificadores.

Para `¿` y `¡`, elige el sistema con **`NAV + B`** (Linux) o **`NAV + T`**
(Windows). Estas combinaciones no escriben nada y afectan solo a los signos
invertidos. El teclado arranca en modo Linux y no guarda el modo en EEPROM.
Linux usa `Ctrl+Shift+U` seguido del valor hexadecimal (`BF` o `A1`); Windows
usa los códigos Alt numéricos `0191` y `0161`. La aplicación debe admitir
la secuencia correspondiente; en Sway no todas aceptan `Ctrl+Shift+U`.

**Prueba tras flashear:** comprueba los cuatro gestos en un campo de texto.
Mantén ambos Shift y pulsa una letra: debe salir en mayúscula, sin añadir
un signo al soltar los Shift. Comprueba también `Shift + Backspace` en
BASE y que `NAV + '` cambia a rojo ambas mitades.

## Colores por capa y Bloq Mayús

Las dos mitades muestran un color fijo de brillo bajo: **BASE violeta**,
**SÍM ámbar** y **NAV azul**. Si mantienes las dos capas, NAV tiene prioridad;
al soltarlas, vuelve el violeta. El mapa usa RGB Matrix y no guarda cambios
de iluminación en la EEPROM. Mientras Bloq Mayús esté activo, el estado
que informa el sistema se sincroniza entre las mitades y **ambas muestran
rojo fijo**, incluso en SÍM o NAV. Al desactivarlo vuelve el color de la
capa activa.

## Español en Emacs

Selecciona `spanish-prefix` con `M-x set-input-method`. Luego puedes alternarlo
con `C-\`: mantén `SÍM` y `Ctrl` y pulsa `P`. Con el método activo, escribe
`'` seguido de una vocal para acentuarla, `~n` para `ñ`, `"u` para `ü`,
`~?` para `¿` y `~!` para `¡`. Esto no activa un método de entrada en `vi`.

## Instalación controlada en Gentoo

Ejecuta los comandos desde la raíz de esta copia de QMK. Necesitas Git, GNU
Make y Python 3 con `venv` y `pip`. Los scripts usan `sh` POSIX. La
instalación de paquetes del sistema y los cambios de reglas udev pueden
requerir permisos de administrador; la compilación y el flasheo no.

### 1. Toolchain AVR

```sh
scripts=keyboards/crkbd/keymaps/emacs_es
sh "$scripts/gentoo-avr.sh" --plan
sh "$scripts/gentoo-avr.sh" --apply
```

`--plan` comprueba si ya tienes un toolchain que reconoce el ATmega32U4.
Si falta, muestra la vista previa de Portage para `sys-devel/crossdev`, o
la configuración del objetivo `avr` si `crossdev` ya está instalado. No
instala nada ni simula la resolución completa de dependencias del toolchain.

`--apply` reutiliza el toolchain si funciona; si no, instala `crossdev` cuando
haga falta y prepara binutils, GCC y avr-libc para AVR. `crossdev` puede
escribir en `/etc/portage` y en su overlay. El overlay predeterminado es
`/var/db/repos/crossdev`; puedes elegir otro con `QMK_AVR_OVERLAY`. No se
instala un toolchain ARM ni herramientas de flasheo. Portage pide
confirmación antes de instalar paquetes.

### 2. QMK en un entorno virtual

```sh
sh "$scripts/qmk-local.sh" --plan
sh "$scripts/qmk-local.sh" --resolve
sh "$scripts/qmk-local.sh" --apply
```

- `--plan` solo muestra las rutas y los pasos.
- `--resolve` crea `.venv` si falta y hace una vista previa de `pip`. Puede
  acceder a la red y descargar metadatos, pero no instala QMK ni las
  dependencias del repositorio.
- `--apply` instala la CLI de QMK y las dependencias de `requirements.txt`
  dentro del entorno. No usa `sudo`, no ejecuta `qmk setup` y no clona otra
  copia del repositorio.

Puedes elegir otro entorno con `QMK_VENV`. Si cambias esa variable, usa el
mismo valor para resolver, instalar y compilar.

### 3. Compilación sin flashear

```sh
git submodule update --init lib/lufa
sh "$scripts/qmk-local.sh" --compile
```

El primer comando inicializa el submódulo USB AVR que usa este destino. La
compilación equivale a `qmk compile -kb crkbd/rev1 -km emacs_es`; el script
usa la CLI del entorno virtual y desactiva la sincronización automática de
submódulos de QMK. Si termina correctamente, genera
`crkbd_rev1_emacs_es.hex`. Compilar no flashea el teclado.

Si ya tenías un `.hex` de una versión anterior, vuelve a compilar antes
de flashear para incorporar los cambios del mapa.

### 4. Flasheo manual, una mitad a la vez

Este Corne se identificó en modo bootloader como `1b4f:9205` (Caterina,
Pro Micro de 5 V/16 MHz). Comprueba `command -v avrdude`. Si falta, puedes
instalarlo con `sudo emerge --ask --verbose dev-embedded/avrdude`.

```sh
sh "$scripts/flash-caterina.sh" --plan
```

Con el USB desconectado, separa las mitades por TRRS. Conecta por USB
directamente a Gentoo solo la mitad que vas a flashear. Ejecuta:

```sh
sh "$scripts/flash-caterina.sh" --apply
```

Cuando aparezca `Esperando Caterina`, pulsa `RESET` dos veces rápido. El
script espera hasta 30 segundos y solo acepta un puerto `ttyACM` cuyo
dispositivo USB tenga el ID `1b4f:9205`. Escribe el `.hex` ya compilado
con `avrdude` y deja activa su verificación por defecto. No instala paquetes,
no usa `sudo` y no cambia la EEPROM.

Cuando `avrdude` termine sin errores, desconecta el USB y repite el proceso
con la otra mitad. Desconecta el USB antes de volver a unirlas por TRRS.

### Si `/dev/ttyACM0` da un error de permisos

Si el script dice `Sin permisos de lectura y escritura`, encontró el
bootloader, pero se detuvo antes de escribir. No necesitas recompilar.

**Sesión gráfica local con `elogind` o `systemd-logind`:** crea
`/etc/udev/rules.d/70-corne-caterina.rules` con `sudoedit` y añade esta línea.
La regla se limita a los puertos serie del bootloader `1b4f:9205`:

```udev
SUBSYSTEM=="tty", KERNEL=="ttyACM*", ATTRS{idVendor}=="1b4f", ATTRS{idProduct}=="9205", TAG+="uaccess"
```

Recarga las reglas y vuelve a ejecutar el script como tu usuario:

```sh
sudo udevadm control --reload-rules
sh "$scripts/flash-caterina.sh" --apply
```

Cuando el script esté esperando, pulsa `RESET` dos veces rápido. Udev
aplicará la regla al puerto cuando vuelva a aparecer.

**SSH o sistema sin logind:** `uaccess` no concede acceso a una sesión
remota. Averigua el propietario y el grupo mientras Caterina esté activo
(usa el puerto que mostró el script si no es `ttyACM0`):

```sh
id
ls -l /dev/ttyACM0
```

Si el grupo del puerto ya tiene lectura y escritura, pide al administrador
que añada tu usuario a ese grupo si corresponde y vuelve a iniciar sesión.
Si no, configura una regla udev para ese mismo ID USB con un grupo dedicado.
No abras el puerto a todos los usuarios ni ejecutes el script con `sudo`.

## Referencia para mantenimiento

- [`keymap.c`](keymap.c) define las tres capas de `LAYOUT_split_3x6_3`, con
  42 posiciones por capa. Si cambia el mapa, actualiza también los diagramas
  de este README. El mapa `default` no se modificó.
- El objetivo elegido es `crkbd/rev1`: el USB anuncia `4653:0001`, que
  también usa `crkbd/r2g`, pero este teclado tiene una controladora separada
  tipo Pro Micro y se verificó que el chip es un ATmega32U4. El ID USB y el
  chip no identifican el bootloader; Caterina se confirmó por separado
  después de un doble `RESET`.
- Los scripts son [`gentoo-avr.sh`](gentoo-avr.sh),
  [`qmk-local.sh`](qmk-local.sh) y [`flash-caterina.sh`](flash-caterina.sh).
  Para comprobar sintaxis sin instalar ni flashear nada:

```sh
sh -n keyboards/crkbd/keymaps/emacs_es/gentoo-avr.sh
sh -n keyboards/crkbd/keymaps/emacs_es/qmk-local.sh
sh -n keyboards/crkbd/keymaps/emacs_es/flash-caterina.sh
```

Se verificó el número de posiciones de las capas y la sintaxis de los
scripts. La compilación se completó en Gentoo y el script detectó Caterina
en `/dev/ttyACM0`, pero el primer intento se detuvo por permisos antes de
escribir el firmware.
