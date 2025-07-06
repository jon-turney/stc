tests = {
  'wcstombs-utf8' : 'wcstombs-utf8.c',
  'wmemset': 'wmemset_test.c',
}

foreach name, sources : tests
 e = executable(name, sources)
 test(name, e)
endforeach

