tests = {
  'random_filenames': 'random_filenames.c',
  'socketpair_full': 'socketpair_full.c',
}

foreach name, sources : tests
 e = executable(name, sources)
 test(name, e, should_fail: true)
endforeach
