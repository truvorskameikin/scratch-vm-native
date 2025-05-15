# scratch-vm-native
```
black scratch-transpiler.py && python3 scratch-transpiler.py --input digital_clock.sb3 --output=digital_clock && gcc digital_clock.c digital_clock_test.c -o digital_clock_test && ./digital_clock_test
```

Build:
```
meson setup builddir/
ninja -C builddir/ digital_clock
```

Debug in VS Code:

Install CodeLLDB plugin and add debug configuration in launch.json:
```
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "lldb",
            "request": "launch",
            "name": "Debug digital_clock",
            "program": "${workspaceFolder}/builddir/digital_clock",
            "args": [],
            "cwd": "${workspaceFolder}"
        }
    ]
}
```

Set breakpoint on `main`.

Run debugger.