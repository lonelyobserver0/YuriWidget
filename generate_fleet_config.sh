#!/bin/bash

mkdir -p .fleet

cat > .fleet/workspace.json <<EOF
{
  "settings": {
    "c": {
      "compiler": "gcc",
      "compilerArgs": [
EOF

# Aggiungi ogni include tra virgolette e con virgola finale
for arg in -I/usr/include/webkitgtk-4.0 -I/usr/include/gtk-3.0 -I/usr/include/pango-1.0 -I/usr/include/cloudproviders -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/atk-1.0 -I/usr/include/dbus-1.0 -I/usr/lib/dbus-1.0/include -I/usr/include/fribidi -I/usr/include/pixman-1 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gio-unix-2.0 -I/usr/include/libsoup-2.4 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -pthread; do
    echo "        \"$arg\"," >> .fleet/workspace.json
done

cat >> .fleet/workspace.json <<EOF
      ],
      "linkerArgs": [
        "-lgtk-3",
        "-lwebkit2gtk-4.0"
      ]
    }
  }
}
EOF

echo "✅ .fleet/workspace.json generato con successo!"
