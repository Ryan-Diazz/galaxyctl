#!/usr/bin/env bash
# Build a .deb of galaxyctl from the current source tree.
#
#   packaging/deb/build.sh <version> [output dir]
#
# Requires: make, a C compiler, dpkg-dev
set -euo pipefail

version=${1:?usage: $0 <version> [output dir]}
outdir=${2:-dist}

name=galaxyctl
arch=$(dpkg --print-architecture)
root=$(mktemp -d)
trap 'rm -rf "$root"' EXIT

make clean
make
make install DESTDIR="$root/pkg" PREFIX=/usr

install -Dm644 LICENCE "$root/pkg/usr/share/doc/$name/copyright"

# Let dpkg-shlibdeps work out the minimum libc version from the binary
mkdir -p "$root/debian"
printf 'Source: %s\n\nPackage: %s\nArchitecture: any\n' "$name" "$name" > "$root/debian/control"
depends=$(cd "$root" && dpkg-shlibdeps -O "pkg/usr/bin/$name" | sed -n 's/^shlibs:Depends=//p')

mkdir -p "$root/pkg/DEBIAN"
cat > "$root/pkg/DEBIAN/control" <<EOF
Package: $name
Version: $version
Architecture: $arch
Maintainer: Ryan-Diazz <https://github.com/Ryan-Diazz/galaxyctl>
Depends: $depends
Recommends: bash-completion
Section: utils
Priority: optional
Homepage: https://github.com/Ryan-Diazz/galaxyctl
Description: Control Samsung Galaxy Book firmware features
 Command-line utility for the samsung-galaxybook kernel driver (Linux 6.15+):
 webcam recording block, power on lid open, power profile and battery
 charge limit / health.
EOF

mkdir -p "$outdir"
dpkg-deb --root-owner-group --build "$root/pkg" "$outdir/${name}_${version}_${arch}.deb"
