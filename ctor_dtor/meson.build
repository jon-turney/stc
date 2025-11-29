dll3 = shared_library('dll3', sources: 'dll3.c', name_prefix: '')
dll2 = shared_library('dll2', sources: 'dll2.c', name_prefix: '')

t = executable('dll_dtor', sources: 'dll_dtor.c',link_with: dll2)
test('dll_dtor', t)
