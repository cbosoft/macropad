#!/usr/bin/env bash
function error() {
  echo "$1"
  exit 1
}

DEST_NAME="$(basename $PWD).uf2"
docker run --rm -v ./.build:/work/build -it $(docker build -q .) && cp .build/main.uf2 "$DEST_NAME" || error "failed to build, see above."

echo "Done! UF2 copied to \"$DEST_NAME\""
