#!/usr/bin/sh

rm ./sigsegv.exe.core

# coredump are off by default
${SIGSEGV}
status=$?

if [ $status != 139 ]
then
  echo "exit code $status != 128 + signal number"
  exit 1
fi

if [ -f "./sigsegv.exe.core" ]
then
  echo "coredump written but should be disabled by default"
  exit 1
fi

# enable coredumps
ulimit -c unlimited
${SIGSEGV}
status=$?

if [ $status != 139 ]
then
  echo "exit code $status != 128 + signal number"
  exit 1
fi

if [ ! -f "./sigsegv.exe.core" ]
then
  echo "coredump not written when enabled"
  exit 1
fi
