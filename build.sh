#!/usr/bin/env bash
docker run --rm -v ./.build:/work/build -it $(docker build -q .) && cp _build/main.uf2 "$(basename $PWD).uf2"
