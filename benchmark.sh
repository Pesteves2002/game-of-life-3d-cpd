#!/usr/bin/env bash

TYPE=$1
CPUs=${2:-1,2,4,6}
ARGUMENTS=${@:3}
ARGUMENTS=${ARGUMENTS:-1 1024 .4 100}

GREEN='\033[0;32m'
CLEAR='\033[0m'

if [ -z $TYPE ]; then
  echo -e "Usage: $0 <type> [cpus] [arguments]"
  echo -e "  type: omp or mpi"
  echo -e "  cpus: comma separated list of CPUs to benchmark"
  echo -e "  arguments: arguments to pass to the program"
  exit 1
fi

debug() {
  if [ "$DEBUG" = "true" ]; then
    echo -e $1 1>&2
  fi
}

compile_program() {
  debug "Compiling $1"
  (
    pushd $1
    make clean && make > /dev/null
    popd
  ) > /dev/null
  debug "Done compiling $1"
}

run_program() {
  result=$($1 $ARGUMENTS 2>&1)

  # Extract the time from the result first line
  time=$(echo $result | awk '{print $1}' | sed 's/s//')
  debug "Took $time seconds with $cpu CPUs"

  echo $time
}

calculate_speedup() {
  speedup=`echo "scale=3; $1 / $2" | bc`
  echo $speedup
}

benchmark() {
  serial_results=$(run_program serial/life3d)
  debug "Serial: $serial_results"

  for cpu in $(echo $CPUs | sed "s/,/ /g"); do

      debug "Running with $cpu CPUs"
      export OMP_NUM_THREADS=$cpu
      debug "OMP_NUM_THREADS=$OMP_NUM_THREADS"

      paralel_results=$(run_program "$1/life3d-$1")
      debug "Paralel ($1) with $cpu CPUs: $paralel_results"

      unset OMP_NUM_THREADS

      speedup=$(calculate_speedup $serial_results $paralel_results)
      debug "Speedup: $speedup"

      echo "$cpu $speedup"
  done
}


debug "Running with $CPUs CPUs"
debug "Arguments: $ARGUMENTS"

# compile_program serial
# compile_program $TYPE

benchmark $TYPE
