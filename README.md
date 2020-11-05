# doggo-ninja-cpp
A c++ CLI interface to doggo.ninja.
# Requirements
- C++ compiler
- make
- libcurl (currently looking for windows alternative)
# Building
```
git clone https://github.com/linuxnoodle/doggo-ninja-cpp/ && cd doggo-ninja-cpp && make
```
# Usage
- `./doggo-ninja -h`                Brings up help screen.
- `./doggo-ninja -t [token]`        Saves your token to a file.
- `./doggo.ninja -u [filename]`     Uploads a file.
# TODO
- Fix Windows support
- Add ability to upload from clipboard
