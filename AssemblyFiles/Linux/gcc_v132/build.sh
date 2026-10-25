#!/bin/sh
# Run the wizard Makefiles with the loader supplied with the Eco toolchain.
set -eu
eco_build_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
eco_project_dir=$(CDPATH= cd -- "$eco_build_dir/../../.." && pwd)
eco_build_script="$eco_build_dir/build.sh"
eco_integral_build_dir="$eco_project_dir/Components/EcoIntegralSimpson/AssemblyFiles/Linux/gcc_v132"
ECO_FRAMEWORK=${ECO_FRAMEWORK:-$HOME/Dev/Eco.Framework}
ECO_TOOLCHAIN=${ECO_TOOLCHAIN:-$HOME/Dev/toolchain}
export ECO_FRAMEWORK ECO_TOOLCHAIN
eco_gcc_root="$ECO_TOOLCHAIN/eco-x86_64-linux-gnu-gcc-13.2.0"
eco_make_root="$ECO_TOOLCHAIN/eco-gnu-make-4.4.1"
eco_loader="$eco_gcc_root/lib/ld-linux-x86-64.so.2"
eco_library_path="$eco_gcc_root/lib:$eco_gcc_root/lib64"
eco_output_dir="$eco_project_dir/BuildFiles/Linux/x86_64/StaticRelease"
eco_check_dir="$eco_project_dir/BuildFiles/verification"

eco_make() {
    "$eco_loader" --library-path "$eco_library_path" "$eco_make_root/bin/make" \
        --no-print-directory -C "$eco_build_dir" "$@" \
        CC_c="$eco_build_script --cc" AR=ar
}

eco_integral_make() {
    eco_make -C "$eco_integral_build_dir" "$@"
}

case "${1:-build}" in
    --cc)
        shift
        exec "$eco_loader" --library-path "$eco_library_path" \
            "$eco_gcc_root/bin/x86_64-linux-gnu-gcc" --sysroot="$eco_gcc_root" "$@"
        ;;
    --make)
        shift
        eco_make "$@"
        exit
        ;;
    run)
        exec "$eco_loader" --library-path "$eco_library_path" \
            "$eco_output_dir/EcoIntegralSimpson"
        ;;
    lab2-run)
        exec "$eco_loader" --library-path "$eco_library_path" \
            "$eco_output_dir/EcoLab2UnitTest"
        ;;
    clean)
        eco_integral_make -f Makefile clean TARGET=1 ARCH=x86_64 DEBUG=0
        eco_integral_make -f MakefileExe clean TARGET=1 ARCH=x86_64 DEBUG=0
        eco_make -f MakefileLab2 clean
        exit
        ;;
    lab2-test)
        eco_integral_make -f Makefile TARGET=1 ARCH=x86_64 DEBUG=0
        eco_make -f MakefileLab2 all
        mkdir -p "$eco_check_dir"
        "$eco_loader" --library-path "$eco_library_path" \
            "$eco_output_dir/EcoLab2UnitTest" > "$eco_check_dir/lab2-unit-test.log"
        tail -n 3 "$eco_check_dir/lab2-unit-test.log"
        exit
        ;;
    build|test|verify)
        eco_action=${1:-build}
        ;;
    *)
        echo 'Usage: build.sh [build|run|test|lab2-test|lab2-run|verify|clean]' >&2
        exit 2
        ;;
esac

eco_make -f MakefileLab2 calculators
eco_integral_make -f Makefile TARGET=1 ARCH=x86_64 DEBUG=0
eco_integral_make -f MakefileExe TARGET=1 ARCH=x86_64 DEBUG=0
eco_make -f MakefileLab2 all
if [ "$eco_action" = build ]; then
    exit
fi
mkdir -p "$eco_check_dir"
"$eco_loader" --library-path "$eco_library_path" \
    "$eco_output_dir/EcoIntegralSimpson" > "$eco_check_dir/unit-test.log"
tail -n 5 "$eco_check_dir/unit-test.log"
if [ "$eco_action" = verify ]; then
    "$eco_loader" --library-path "$eco_library_path" \
        "$eco_output_dir/EcoLab2UnitTest" > "$eco_check_dir/lab2-unit-test.log"
    tail -n 3 "$eco_check_dir/lab2-unit-test.log"
    python3 "$eco_project_dir/UnitTestFiles/test_reference.py"
    python3 "$eco_project_dir/UnitTestFiles/compare_reference.py"
    python3 "$eco_project_dir/UnitTestFiles/check_objects.py"
fi
