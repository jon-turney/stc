tests = {
  'realloc': 'realloc_fortify.c',
}

foreach name, sources : tests
 e = executable(name, sources, c_args: ['-O2', '-D_FORTIFY_SOURCE=2'])
 test(name, e)
endforeach
