#!/usr/bin/env bash
# Compute the next version from conventional commits since the last v* tag.
#
#   feat!: / fix!: / BREAKING CHANGE  -> major
#   feat:                             -> minor
#   anything else                     -> patch
#   no commits since the last tag     -> no release
#
# Prints key=value lines (suitable for $GITHUB_OUTPUT):
#   previous=<last tag or empty>
#   version=<X.Y.Z>
#   bump=<major|minor|patch|none>
set -euo pipefail

last_tag=$(git describe --tags --abbrev=0 --match 'v[0-9]*.[0-9]*.[0-9]*' 2>/dev/null || true)

if [[ -n $last_tag ]]; then
    range="$last_tag..HEAD"
    IFS=. read -r major minor patch <<< "${last_tag#v}"
else
    range="HEAD"
    major=1 minor=0 patch=0
fi

subjects=$(git log --format='%s' "$range")
bodies=$(git log --format='%b' "$range")

if [[ -z $subjects ]]; then
    bump=none
elif grep -qE '^[a-zA-Z]+(\([^)]*\))?!:' <<< "$subjects" || grep -q 'BREAKING CHANGE' <<< "$bodies"; then
    bump=major
elif grep -qE '^feat(\([^)]*\))?:' <<< "$subjects"; then
    bump=minor
else
    bump=patch
fi

case $bump in
    major) major=$((major + 1)) minor=0 patch=0 ;;
    minor) minor=$((minor + 1)) patch=0 ;;
    patch) patch=$((patch + 1)) ;;
esac

echo "previous=$last_tag"
echo "version=$major.$minor.$patch"
echo "bump=$bump"
