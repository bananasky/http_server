#!/bin/bash

pwd0="$(pwd)"

fname="WebServer"

arg1="$1"

tput smul; tput bold; echo "   Preparation   "; tput sgr0

# Looking for source file
for ext in py java c cpp; do
  for srcpath0 in $(ls "$fname"-*."$ext" 2>/dev/null); do
    srcpath="$srcpath0"
    srclang="$ext"
    break
  done
  if [[ -n "$srcpath" ]]; then
    break
  fi
done
if [[ -z "$srcpath" ]]; then
  echo "Error: cannot find source file for $fname"
  exit 1
fi
echo "Info: found source file $srcpath"

# Checking OS
if ! grep '^Linux stu' < <(uname -a) >/dev/null; then
  echo "Warn: not running on stu!"
fi

findPy3() {
  if [[ ! -z "$py3" ]]; then
    return
  fi
  py3="/usr/bin/python3"
  if [[ ! -x "$py3" ]]; then
    py3="$(which python3)"
    if [[ ! -x "$py3" ]]; then
      echo "Error: cannot find python3"
      py3=""
      return 1
    else
      echo "Warn: using python3 @ $py3"
    fi
  fi
}

mkdirBuild() {
  if [[ ! -z "$build_dir" ]]; then
    return
  fi
  build_dir="build"
  mkdir -p "$build_dir"
  echo "Info: created build directory $build_dir"
#  trap "kill -9 \$(cat \"$build_dir/pids\" 2>/dev/null) 2>/dev/null; rm -rf \"$build_dir\"" EXIT INT TERM KILL
}

cmakeBuild() {
  if [ -d "$build_dir" ]; then
      echo "Cleaning build directory..."
      rm -rf "$build_dir"
  fi
  echo "Info: Building using CMake..."
  cmake -S . -B "$build_dir" || { echo "Error: CMake build failed!"; exit 3; }
  cmake --build "$build_dir" || { echo "Error: Compilation failed!"; exit 3; }
  # Set evalcmd to the compiled binary location
  evalcmd="$build_dir/$fname"
}

evalRun() {
  local rid="$1"
  shift 1
  eval "$@" > "$build_dir/$rid.stdout"
}

launchTest() {
  cdir="$pwd0/cases/$fname"
  let pcases=0
  if [[ -z "$arg1" ]]; then
    let cid=1
    while [[ -f "$cdir/$cid.in" ]]; do
      echo "-- Test Case $cid --"
      "$1" "$cid"
      let cid++
    done
    echo "-- Summary --"
    echo "Passed $pcases out of $((cid-1)) cases."
  else
    cid="$arg1"
    if [[ -f "$cdir/$cid.in" ]]; then
      echo "-- Test Case $cid --"
      "$1" "$cid"
    else
      echo "Error: invalid case id \"$cid\""
      exit 4
    fi
  fi
}
