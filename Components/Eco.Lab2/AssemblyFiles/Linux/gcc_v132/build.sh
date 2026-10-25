#!/bin/sh
# The laboratory's common build registers and links every required component.
set -eu
lab2_build_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
lab2_project_dir=$(CDPATH= cd -- "$lab2_build_dir/../../../../.." && pwd)
if [ "$#" -eq 0 ]; then
    set -- lab2-test
fi
exec "$lab2_project_dir/AssemblyFiles/Linux/gcc_v132/build.sh" "$@"
