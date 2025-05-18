#!/bin/bash

echo ""
echo "--------------------------------------------------"
echo ""


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
echo "Pango Directory: $pango_path"
pango_dir="${pango_path%/*}"
echo "Pango Directory: $pango_dir"

# Trova il percorso della directory di pango.h
gdk_pixbuf_path=$(find_lib gdk-pixbuf.h)
echo "GDK-Pixbuf Directory: $gdk_pixbuf_path"
gdk_pixbuf_dir="${gdk_pixbuf_path%/*}"
echo "GDK-Pixbuf Directory: $gdk_pixbuf_dir"

# Elenco delle librerie da cercare
librerie=("gtk4-layer-shell.h" "glib.h" "cairo.h" "hb.h" "graphene.h")

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
echo ""
echo "--------------------------------------------------"
echo ""


gcc -o yuriwidget yuriwidget.c "-I/usr/lib/glib-2.0/include" "-I/usr/lib/graphene-1.0/include" "-I$gtk4_dir" "-I$pango_dir" "-I$gdk_pixbuf_dir" $opzioni_inclusione
