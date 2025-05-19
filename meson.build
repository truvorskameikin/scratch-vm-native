project(
    'scratch-vm',
    'c',
    'cpp',
    version: '0.0.1',
    meson_version: '>= 0.60',
    license: 'MIT',
    default_options: [
        'c_std=gnu11',
        'cpp_std=c++2a',
        'warning_level=3',
        'werror=true',
    ],
)

fs = import('fs')
inc = include_directories('.')

add_project_arguments('-Wno-unused-parameter', language: ['c', 'cpp'])

python = find_program('python3')

# Required Python packages
required_modules = ['jinja2']

foreach module : required_modules
    res = run_command([python, '-c', 'import ' + module], capture: false, check: false)
    if res.returncode() != 0
        error('Python module `' + module + '` is not found')
    endif
endforeach

# Describe the test using a dictionary with following mandatory fields:
#   - name: binary and build target name
#   - scratch_program: the full path to the translated Scratch program.
#   - sources: any additional source files required for building the test binary. 
digital_clock_test = {
    'name': 'digital_clock_test',
    'scratch_program' : meson.project_source_root() / 'digital_clock.sb3',
    'sources' : [ 'tests/digital_clock_test.c' ]
}

# Build error: 
# repeat_until_x.c:111:37: error: ‘None’ undeclared (first use in this function)
# 111 |   red_100x70_motion_sety2.op_code = None;
# repeat_until_x_test = {
#     'scratch_program' : meson.project_source_root() / 'repeat_until_x.sb3',
#     'sources' : [ 'tests/repeat_until_x_test.c' ]
# }

gen_tests = [ 
    digital_clock_test,
    #repeat_until_x_test
]

foreach test_target : gen_tests
    exe_name = test_target['name']
    sb_prog = test_target['scratch_program']
    bin_sources = test_target['sources']

    base_name = fs.replace_suffix(fs.name(sb_prog), '')
    message('Generate: ' + exe_name)

    gen = custom_target(
        'gen_' + base_name,
        command: [python, meson.project_source_root() / 'scratch-transpiler.py', '-i', sb_prog, '-o', base_name],
        input: sb_prog,
        output: [base_name + '.c', base_name + '.h'],
        depend_files: [ meson.project_source_root() / 'scratch-transpiler.py' ]
    )

    gen_exe = executable(
        exe_name,
        gen.to_list() + bin_sources,
        include_directories: inc,
    )

endforeach
