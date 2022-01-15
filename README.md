# Classy

Classy is a web-based sticky notes app that prioritizes simplicity and performance -- because software should move at least as quickly as you do.

Classy's backend is written in C++ using [Crow](https://crowcpp.org/) as a web microframework, and [SOCI](http://soci.sourceforge.net/) for database handling. Classy's frontend is implemented with [React](https://reactjs.org/). Special thanks to [pr0f3ss](https://github.com/pr0f3ss/SHA512) for a C++ SHA512 implementation.

Key features:
- **High-performance:** Classy's C++ stack yields superior performance to interpreted languages' web frameworks
- **Cloud-based:** Access your notes from any device
- **Privacy:** No 3rd-party account linking

## Demo
You are free to host your own Classy instance (the included Dockerfile may prove useful). For a quick demo, here's my own [instance](http://35.238.221.70:8080) hosted on a [Google Cloud Engine](https://cloud.google.com/compute) VM.

## License
[MIT](https://choosealicense.com/licenses/mit/)