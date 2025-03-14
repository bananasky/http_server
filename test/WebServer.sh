#!/bin/bash

d0="$(dirname "$(readlink -f -- "$0")")"

source "$d0/common.sh"

findPy3
mkdirBuild
cmakeBuild

py38="/usr/bin/python3"

genRandPorts() {
  echo $$ | gawk '{ srand(systime()+$0*100) }END{ print int(rand()*(65534-1025))+1025; print int(rand()*(65534-1025))+1025; print int(rand()*(65534-1025))+1025 }'
}

nCases=$("$py38" -c "from cases1 import *; print(len(casegrps))")

caseA() {
  local i="$1"
  let i=i
  [[ "$i" -le 0 ]] && i=1
  #
  echo
	desc=$("$py38" -c "from cases1 import *; print(casegrps[$(($i-1))][0])")
  tput smul; tput bold; echo "   Test Case ${i}a - ${desc} - one connection per request   "; tput sgr0
  let i--
	nreqs=$("$py38" -c "from cases1 import *; print(len(casegrps[$i])//2)")

  isPassed=1
  # try to launch server
  for p0 in $(genRandPorts); do
    port="$p0"
    rm -f "$build_dir/s-err"
    (eval exec "$evalcmd" $port 2> "$build_dir/s-err") &
    spid="$!"
    echo $spid >> "$build_dir/pids"
    #
    "$py3" -c 'import time; time.sleep(0.3)'
    if ! kill -0 $spid 2>/dev/null; then
      port=""
      continue
    fi
    #
    rm -f "$build_dir/c0-err"
    "$py38" -c "import client" "$port" "$i" 0 0 2> "$build_dir/c0-err"
    rtn="$?"
    if [[ "$rtn" == 1 ]]; then
      port=""
      kill -9 $spid 2>/dev/null
      continue
    else
      break
    fi
  done
  if [[ -z "$port" ]]; then
    tput rev; echo "Error: your server should be ready to accept a new connection. Check that it is running correctly."; tput sgr0
    cat "$build_dir/s-err"
    exit 1
  fi
  if [[ "$rtn" -gt 1 ]]; then
    isPassed=0
    kill -9 $spid 2>/dev/null
    tput rev
    printf "%s" "Failed: "
    cat "$build_dir/c0-err"
    tput sgr0
    cat "$build_dir/s-err"
  fi
  for j in $( seq 1 $((nreqs-1)) ); do
    echo 'seq' $j
    clog="$build_dir/c$j-err"
    rm -f "$clog"
    "$py38" -c "import client" "$port" "$i" "$j" 0 2> "$clog"
    rtn="$?"
    if ! kill -0 $spid 2>/dev/null || [[ "$rtn" -gt 0 ]]; then
      isPassed=0
      kill -9 $spid 2>/dev/null
      tput rev
      printf "%s" "Failed: "
      cat "$clog"
      tput sgr0
      cat "$build_dir/s-err"
      break
    fi
  done
  if [[ "$isPassed" == 1 ]]; then
    tput rev; echo "Passed!"; tput sgr0
		kill -9 $spid 2>/dev/null
  fi
}

caseBCD() {
  local i="$1"
  let i=i
  [[ "$i" -le 0 ]] && i=1
  local ver="$2"
  if [[ "$ver" == b ]]; then
    local cver=''
    casedescr='persistent connection, batched'
  elif [[ "$ver" == c ]]; then
    local cver='3'
    casedescr='persistent connection, ping-pong'
  else
    local cver='2'
    i=$(( nCases+1 ))
    casedescr='intermittent connection'
  fi
  #
  echo
  if [[ "$i" -le "$nCases" ]]; then
	  desc=$("$py38" -c "from cases1 import *; print(casegrps[$(($i-1))][0])")
  else
    desc='ALL (except large content body)'
  fi
  tput smul; tput bold; echo "   Test Case ${i}$ver - ${desc} - $casedescr   "; tput sgr0
  let i--
  # try to launch server
  for p0 in $(genRandPorts); do
    port="$p0"
    rm -f "$build_dir/s-err"
    (eval exec "$evalcmd" $port 2> "$build_dir/s-err") &
    spid="$!"
    echo $spid >> "$build_dir/pids"
    #
    "$py3" -c 'import time; time.sleep(0.3)'
    if ! kill -0 $spid 2>/dev/null; then
      port=""
      continue
    fi
    #
    rm -f "$build_dir/c-err"
    "$py38" -c "import client${cver}" "$port" "$i" -1 0 2> "$build_dir/c-err"
    rtn="$?"
    if [[ "$rtn" == 1 ]]; then
      port=""
      kill -9 $spid 2>/dev/null
      continue
    else
      break
    fi
  done
  if [[ -z "$port" ]]; then
    tput rev; echo "Error: cannot allocate a port number for your server. Try again later?"; tput sgr0
    cat "$build_dir/s-err"
    exit 1
  fi
  if [[ "$rtn" == 0 ]]; then
    tput rev; echo "Passed!"; tput sgr0
		kill -9 $spid 2>/dev/null
  else
    tput rev
    printf "%s" "Failed: "
    cat "$build_dir/c-err"
    tput sgr0
    cat "$build_dir/s-err"
		kill -9 $spid 2>/dev/null
  fi
}


######################3

if [[ -z "$arg1" ]]; then
  for ii in $(seq 1 "$nCases"); do
    caseA $ii
    caseBCD $ii b
    caseBCD $ii c
  done
  caseBCD
else
  slen=${#arg1}
  tid=${arg1::((slen-1))}
  lastc=${arg1:((slen-1))}
  if [[ "$arg1" == $(( nCases+1 )) ]]; then
    caseBCD
  elif [[ "$lastc" == a ]]; then
    caseA $tid
  elif [[ "$lastc" == b ]]; then
    caseBCD $tid b
  elif [[ "$lastc" == c ]]; then
    caseBCD $tid c
  elif grep '[0-9]' <(echo $lastc) >/dev/null; then
    caseA $arg1
    caseBCD $arg1 b
    caseBCD $arg1 c
  else
    echo "Unknown test case: \"$arg1\""
  fi
fi
