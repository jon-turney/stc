tests = {
  'many_files': 'many_files.c',
  'many_locks': 'many_locks.c',
  'flock1': 'flock1.c',
  'flock2': 'flock2.c',
}

foreach name, sources : tests
 e = executable(name, sources)
 test(name, e, timeout: 120)
endforeach
