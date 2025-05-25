tests = {
  'm_pi': 'm_pi_test.c',
  'ctype_precedence': 'ctype_precedence.c',
}

foreach name, sources : tests
 e = executable(name, sources)
 test(name, e)
endforeach

foreach std : ['c89', 'c99', 'c11', 'c17', 'c2x', 'gnu89', 'gnu99', 'gnu11', 'gnu17', 'gnu2x']
 static_library(std, 'all_headers.c', c_args : ['-std='+std, '-D_GNU_SOURCE'])
endforeach
