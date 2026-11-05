# Light monitor

C++14 console application for the Josh.ai lights challenge. It uses only the
standard library, system networking, and two vendored header-only dependencies.
No dependency downloads or third-party shared libraries are required to build.

## Run a prebuilt binary

Start the Lights Simulator separately, then run the binary for your platform:

```sh
# Apple Silicon Mac (ARM64)
./bin/macos-arm64/light_monitor

# Ubuntu 24.04 ARM64 (AArch64)
./bin/linux-arm64/light_monitor
```

Stop the monitor with **Ctrl+C**. The program uses the operating system’s default
signal behavior.

## Configuration

Environment variable | Default | Meaning
--- | --- | ---
`LIGHTS_HOST` | `127.0.0.1` | Simulator hostname or IP address, without a URL scheme
`LIGHTS_PORT` | `8080` | Simulator port, integer from 1 to 65535
`LIGHTS_POLL_MS` | `1000` | Delay in milliseconds between successful polling cycles; positive integer up to 2147483647

```sh
LIGHTS_HOST=127.0.0.1 LIGHTS_PORT=8080 LIGHTS_POLL_MS=500 ./bin/macos-arm64/light_monitor
```

Events go to stdout, flushed immediately even when redirected. Diagnostics are
JSON objects on stderr, so they can be captured separately:

```sh
./bin/macos-arm64/light_monitor > events.log 2> errors.log
```

Invalid configuration exits with status 1.

## Build from source

Run these commands from this directory. Requirements: a C++14 compiler and,
for the CMake workflow, CMake 3.14 or later. The dependencies in `include/` are
already provided. On macOS, install Apple's Command Line Tools with
`xcode-select --install`. On Ubuntu, install `build-essential` and `cmake`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/light_monitor
```

## Source layout

- `src/LightsClient.*`: HTTP response validation and light parsing
- `src/LightMonitor.*`: polling, retained state, change detection, retry lifecycle
- `src/ConsoleReporter.*`: event serialization and brightness conversion
- `src/HttpClient.*`: cpp-httplib adapter with connection/read timeouts
- `src/Backoff.*`: bounded retry delay
- `src/main.cpp`: configuration and program setup
- `bin/`: prebuilt monitor binaries
