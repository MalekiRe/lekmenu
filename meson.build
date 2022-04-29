project('lekmenu', 'cpp', version: '0.1', default_options: ['cpp_std=c++17'])
includes = include_directories('include')
fusion = dependency('stardustxr-fusion')
flatbuffers = dependency('flatbuffers')
readline = dependency('readline')
dbus = dependency('sdbus-c++', required : false)
xcb = dependency('xcb')
xcb_xinput = dependency('xcb-xinput')
xkb = dependency('xkbcommon')
xkb_x11 = dependency('xkbcommon-x11')

dependencies = [fusion, flatbuffers, xkb, readline]

files = [
    'main.cpp',
    'main.h',
    'keyboardhandler.cpp',
    'keyboardhandler.h',
    'ApplicationPathHandler.cpp',
    'ApplicationPathHandler.h',
    'Option.cpp',
    'Option.h'
]


executable('lekmenu', files, dependencies: dependencies, include_directories: includes)