if 'sdl' in get_option('frontend')

    sdl2_dep = sdl2.get_variable('sdl2_dep')
    sdl2main_dep = sdl2.get_variable('sdl2main_dep')

    sdl_frontend_lib = static_library(
        'sdl_frontend',
        sources: ['sdl_main.cpp'],
        dependencies: [sdl2_dep, sdl2main_dep],
    )

    sdl_frontend_dep = declare_dependency(
        link_with: sdl_frontend_lib,
        include_directories: include_directories('.'),
    )
else
    sdl_frontend_dep = declare_dependency()
endif
