tests = {
  'm_pi': 'm_pi_test.c',
  'ctype_precedence': 'ctype_precedence.c',
}

foreach name, sources : tests
 e = executable(name, sources)
 test(name, e)
endforeach
