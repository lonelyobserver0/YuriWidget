#!/bin/bash

find_lib() {
    data_gen=$(find /usr/include -name "$1")
    gen_dir="${data_gen%/*}"
    echo "$gen_dir"
}

# Find gtk4 directory path

data=$(pkg-config --cflags --libs gtk4)

gtk4_dir_ext=$(echo "$data" | cut -d ' ' -f 1)

gtk4_dir_short=${gtk4_dir_ext#*/}
gtk4_dir="/$gtk4_dir_short"
echo "$gtk4_dir"

librerie=("gtk4-layer-shell.h" "glib.h" "glibconfig.h")

opzioni_inclusione=""

for libreria in "${librerie[@]}"; do
    libreria_path=$(find_lib "$libreria")  # Trova il percorso della libreria
    opzioni_inclusione="-I$libreria_path $opzioni_inclusione"  # Aggiungi l'opzione di inclusione
done

echo "$opzioni_inclusione"

# Add GTK4 directory to building command

#gcc -o yuriwidget yuriwidget.c "-I$gtk4_dir" "-I$gtk4ls_dir" "-I$glib_dir" `pkg-config --cflags --libs gtk4 gtk-layer-shell`
