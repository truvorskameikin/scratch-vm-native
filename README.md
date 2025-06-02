# scratch-vm-native

## Configure project
```
meson setup builddir/
```
Add the following arguments to enable:
* release build: `-Dbuildtype=release`
* address and undefined behavior sanitizer: `-Db_sanitize=address`

If build was already configured, add `--reconfigure`.
Example:
```
meson setup builddir/ -Db_sanitize=address --reconfigure 
```

## Build a specific target
```
ninja -C builddir/ digital_clock_test
```

## Run all tests
```
ninja -C builddir/ test
```

## Run a specific test
```
ninja -C builddir/ digital_clock_test_run
```

## Debug in VS Code

Set breakpoint.
```
ninja -C builddir/ digital_clock_test_dbg
```
