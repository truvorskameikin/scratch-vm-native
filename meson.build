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
        'werror=false'
    ],
)

# gtest enable
gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_main_dep')

fs = import('fs')
inc = include_directories('.')

python = find_program('python3')

vscode = find_program('codium', 'code', '/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code', required : false)

# Required Python packages
required_modules = ['jinja2']

foreach module : required_modules
    res = run_command([python, '-c', 'import ' + module], capture: false, check: false)
    if res.returncode() != 0
        error('Python module `' + module + '` is not found')
    endif
endforeach

# Describe the test using a dictionary with following fields:
# - mandatory
#   - name: binary and build target name
#   - scratch_program: the full path to the translated Scratch program.
#   - sources: any additional source files required for building the test binary.
# - optional
#   - deps: list of dependencies to test (e.g. gtest_dep for gtest)
digital_clock_test = {
    'name' : 'digital_clock_test',
    'scratch_program' : meson.project_source_root() / 'digital_clock.sb3',
    'sources' : [ 'tests/digital_clock_test.c' ]
}

digital_clock_gtest = {
    'name' : 'digital_clock_gtest',
    'scratch_program' : meson.project_source_root() / 'digital_clock.sb3',
    'sources' : [ 'tests/digital_clock_gtest.cpp' ],
    'deps' : [gtest_dep]
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
    digital_clock_gtest,
    #repeat_until_x_test
]

scratch_gens = {}

foreach test_target : gen_tests
    exe_name = test_target['name']
    sb_prog = test_target['scratch_program']
    bin_sources = test_target['sources']
    deps = test_target.get('deps', [])

    base_name = fs.replace_suffix(fs.name(sb_prog), '')
    message('Generate: ' + exe_name)

    if not scratch_gens.has_key(sb_prog)
        gen = custom_target(
            'gen_' + base_name,
            command: [python, meson.project_source_root() / 'scratch-transpiler.py', '-i', sb_prog, '-o', base_name],
            input: sb_prog,
            output: [base_name + '.c', base_name + '.h'],
            # Regenerate if script or templates changed
            depend_files: [
                meson.project_source_root() / 'scratch-transpiler.py',
                'templates/scratch-transpiler-main-template.c',
                'templates/scratch-transpiler-main-template.h'
            ]
        )
        scratch_gens = scratch_gens + { sb_prog : gen }
    endif

    gen_exe = executable(
        exe_name,
        scratch_gens[sb_prog].to_list() + bin_sources,
        dependencies: deps,
        include_directories: inc,
    )

    # Add the test to meson test list
    test(exe_name, gen_exe)

    # update launch.json on configure
    meson.add_postconf_script(python, meson.project_source_root() / 'scripts' / 'update_launchjson.py', exe_name, gen_exe.full_path())

    # Add run command
    run_target(exe_name + '_run', command: [gen_exe], depends: [gen_exe])

    # Run debugger in vscode (CodeLLDB plugin required)
    if vscode.found()
        run_target(exe_name + '_dbg', command: [vscode, '--open-url', 'vscode://vadimcn.vscode-lldb/launch?name=' + exe_name], depends: [gen_exe])
    endif
endforeach
