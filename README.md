# scratch-vm-native
```
black scratch-transpiler.py && python3 scratch-transpiler.py --input digital_clock.sb3 --output=digital_clock && gcc digital_clock.c digital_clock_test.c -o digital_clock_test && ./digital_clock_test
```

Build:
```
meson setup builddir/
ninja -C builddir/ digital_clock_test
```

Run all tests
```
ninja -C builddir/ test
```

Run a specific test:
```
ninja -C builddir/ digital_clock_test_run
```

Debug in VS Code:

Set breakpoint.
```
ninja -C builddir/ digital_clock_test_dbg
```
