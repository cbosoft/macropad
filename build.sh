#!/usr/bin/env bash
docker run --rm -v ./_build:/work/_build -it $(docker build -q .)
