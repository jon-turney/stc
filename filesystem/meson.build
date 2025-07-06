tests = {
  'many_locks': 'many_locks.c',
}

foreach name, sources : tests
 e = executable(name, sources)
 test(name, e)
endforeach
