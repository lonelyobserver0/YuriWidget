#!/bin/bash

find_lib() {
    data_gen=$(find /usr/include -name "$1" 2>/dev/null)  # Ignora gli errori
    if [[ -n "$data_gen" ]]; then
        gen_dir="${data_gen%/*}"
        echo "$gen_dir"
    else
        echo ""  # Restituisce una stringa vuota se non trovata
    fi
}

# Trova il percorso della directory di GTK4
data=$(pkg-config --cflags --libs gtk4)

gtk4_dir_ext=$(echo "$data" | cut -d ' ' -f 1)
gtk4_dir_short=${gtk4_dir_ext#*/}
gtk4_dir="/$gtk4_dir_short"
echo "GTK4 Directory: $gtk4_dir"

# Trova il percorso della directory di pango.h
pango_path=$(find_lib pango.h)
pango_dir="${path%/*/*}"

# Elenco delle librerie da cercare
librerie=("gtk4-layer-shell.h" "glib.h" "cairo.h")

opzioni_inclusione=""

for libreria in "${librerie[@]}"; do
    libreria_path=$(find_lib "$libreria")
    if [[ -n "$libreria_path" ]]; then
        opzioni_inclusione="-I$libreria_path $opzioni_inclusione"
    else
        echo "Libreria non trovata: $libreria"
    fi
done

echo "Opzioni di inclusione: $opzioni_inclusione"
echo "--------------------------------------------------"

gcc -o yuriwidget yuriwidget.c "-I$gtk4_dir" "-I/usr/lib/glib-2.0/include" "-I$pango_dir" $opzioni_inclusione `pkg-config --cflags --libs gtk4 gtk-layer-shell`
