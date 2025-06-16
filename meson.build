project(
    'scratch-vm',
    'c',
    'cpp',
    version: '0.0.1',
    meson_version: '>= 1.6.0',
    license: 'MIT',
    default_options: [
        'c_std=c17',
        'cpp_std=c++2a',
        'warning_level=3',
        'werror=true',
    ],
)

if meson.get_compiler('cpp').get_id() == 'clang'
    # libc++ uses other assertion mechanism
    add_project_arguments(['-DNDEBUG', '-U_DEBUG'], language: 'cpp')
endif

add_project_arguments(['-Wno-unused-function'], language: 'c')

# gtest enable
gtest = subproject('gtest')
gtest_dep = gtest.get_variable('gtest_main_dep')

if 'sdl' in get_option('frontend')
    sdl2 = subproject(
        'sdl2',
        default_options: [
            'warning_level=0',
            'werror=false',
            'test=false',
            'use_audio=disabled',  # TODO: temporarily disabled
        ],
    )
endif

# subdirs
subdir('frontend')

fs = import('fs')
inc = include_directories('.')

python = find_program('python3')

vscode = find_program(
    'codium',
    'code',
    '/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code',
    required: false,
)

# Required Python packages
required_modules = ['jinja2']

foreach module : required_modules
    res = run_command(
        [python, '-c', 'import ' + module],
        capture: false,
        check: false,
    )
    if res.returncode() != 0
        error('Python module `' + module + '` is not found')
    endif
endforeach

scratch_vm_lib = static_library(
    'scratch_vm_lib',
    ['templates/scratch-vm-variables.c'],
    c_args: '-DSCRATCH_VM_ALLOW_INCLUDES',
)

scratch_vm_lib_tests_exe = executable(
    'scratch_vm_lib_tests_exe',
    ['tests/scratch-vm-variables_gtest.cpp'],
    link_with: [scratch_vm_lib],
    dependencies: [gtest_dep],
)
test('scratch_vm_lib_tests', scratch_vm_lib_tests_exe)

# Describe the binary using a dictionary with following fields:
# - mandatory
#   - name: binary and build target name
#   - scratch_program: the full path to the translated Scratch program.
# - optional
#   - sources: any additional source files required for building the test binary.
#   - deps: list of dependencies to test (e.g. gtest_dep for gtest)
#   - enabled: true/false, true by default
digital_clock_gtest = {
    'name' : 'digital_clock_gtest',
    'scratch_program' : meson.project_source_root() / 'digital_clock.sb3',
    'sources' : ['tests/digital_clock_gtest.cpp'],
    'deps' : [gtest_dep],
}

variables_gtest = {
    'name' : 'variables_gtest',
    'scratch_program' : meson.project_source_root() / 'variables.sb3',
    'sources' : ['tests/variables_gtest.cpp'],
    'deps' : [gtest_dep],
}

variables_sdl = {
    'name' : 'variables_sdl',
    'scratch_program' : meson.project_source_root() / 'variables.sb3',
    'deps' : [sdl_frontend_dep],
    'enabled': 'sdl' in get_option('frontend'),
}

gen_targets = [
    # TODO(truvorskameikin): Re-enable after fixing.
    # digital_clock_gtest,
    variables_gtest,
    variables_sdl,
]

scratch_gens = {}

foreach target : gen_targets
    if not target.get('enabled', true)
        continue
    endif

    exe_name = target['name']
    sb_prog = target['scratch_program']
    bin_sources = target.get('sources', [])
    deps = target.get('deps', [])

    base_name = fs.replace_suffix(fs.name(sb_prog), '')
    message('Generate: ' + exe_name)

    if not scratch_gens.has_key(sb_prog)
        gen = custom_target(
            'gen_' + base_name,
            command: [
                python,
                meson.project_source_root() / 'scratch-transpiler.py',
                '-i',
                sb_prog,
                '-o',
                base_name,
            ],
            input: sb_prog,
            output: [base_name + '.c', base_name + '.h'],
            # Regenerate if script or templates changed
            depend_files: [
                meson.project_source_root() / 'scratch-transpiler.py',
                'templates/scratch-transpiler-main-template.c',
                'templates/scratch-transpiler-main-template.h',
            ],
        )
        scratch_gens = scratch_gens + {sb_prog: gen}
    endif

    gen_exe = executable(
        exe_name,
        scratch_gens[sb_prog].to_list() + bin_sources,
        dependencies: deps,
        include_directories: inc,
    )

    # Add the test to meson test list
    if exe_name.contains('test')
        test(exe_name, gen_exe)
    endif

    # update launch.json on configure
    meson.add_postconf_script(
        python,
        meson.project_source_root() / 'scripts' / 'update_launchjson.py',
        exe_name,
        gen_exe.full_path(),
    )

    # Add run command
    run_target(exe_name + '_run', command: [gen_exe], depends: [gen_exe])

    # Run debugger in vscode (CodeLLDB plugin required)
    if vscode.found()
        run_target(
            exe_name + '_dbg',
            command: [
                vscode,
                '--open-url',
                'vscode://vadimcn.vscode-lldb/launch?name=' + exe_name,
            ],
            depends: [gen_exe],
        )
    endif
endforeach
