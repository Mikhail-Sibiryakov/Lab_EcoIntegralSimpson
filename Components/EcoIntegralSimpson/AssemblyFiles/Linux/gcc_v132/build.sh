#!/bin/sh
# Use the laboratory build to supply the shared calculator dependencies.
set -eu
integral_build_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
integral_project_dir=$(CDPATH= cd -- "$integral_build_dir/../../../../.." && pwd)
if [ "$#" -eq 0 ]; then
    set -- build
fi
exec "$integral_project_dir/AssemblyFiles/Linux/gcc_v132/build.sh" "$@"
