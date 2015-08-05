#!/bin/bash

# Copy, then move the file so it appears instantaneously to the
# program. Otherwise, the program may do a partial read during the
# copy.

set -e
IMAGE=${1:-skull}
cp images/$IMAGE.rgb attack-load.rgb
mv attack-load.rgb attack.rgb

