
sigsegv = executable('sigsegv', 'sigsegv.c')

# verify basic coredump functionality
coredump_sh = find_program('coredump.sh')
e = environment({'SIGSEGV': sigsegv.full_path()})
test('coredump', coredump_sh, env: e)

# verify that an already attached debugger (i.e. strace) doesn't put us into an infinite loop on segfault
# https://cygwin.com/pipermail/cygwin/2025-May/258144.html
strace = find_program('strace')
test('strace-sigsegv', strace, args: sigsegv)
