# doggo-ninja-cpp
A c++ CLI interface to doggo.ninja.
# Requirements
- C++ compiler
- libcurl
# Building
```g++ doggo-ninja.cpp -o doggo-ninja -lcurl -fpermissive```
# Usage
- `./doggo-ninja -h`                Brings up this screen.
- `./doggo-ninja -t [token]`        Saves your token to a file.
- `./doggo.ninja -u [filename]`     Uploads a file.
