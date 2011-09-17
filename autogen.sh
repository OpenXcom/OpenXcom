#!/bin/sh
mkdir --parent --verbose m4
aclocal --install -Im4
autoreconf --verbose --install --symlink --force
