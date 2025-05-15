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


scratch_programs = [ 
    meson.project_source_root() / 'digital_clock.sb3',
    
    # Build error: 
    # repeat_until_x.c:111:37: error: ‘None’ undeclared (first use in this function)
    # 111 |   red_100x70_motion_sety2.op_code = None;
    # meson.project_source_root() / 'repeat_until_x.sb3',
    # meson.project_source_root() / 'test_program.sb3'
]

foreach sb_prog : scratch_programs
    file_name = fs.name(sb_prog)
    base_name = fs.replace_suffix(file_name, '')
    message('Generate: ' + base_name)

    gen = custom_target(
        'gen_' + base_name,
        command: [python, meson.project_source_root() / 'scratch-transpiler.py', '-i', sb_prog, '-o', base_name],
        input: sb_prog,
        output: [base_name + '.c', base_name + '.h', base_name + '_test.c']
    )

    gen_exe = executable(
        base_name,
        gen.to_list(),
        include_directories: inc
    )

endforeach
