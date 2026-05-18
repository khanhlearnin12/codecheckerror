# C/C++ Code Checker GUI 🚀

Thanks to: `QT6`, `clang-tidy` , `cppcheck`, `flawfinder`, `valgrind` ,`clang-format` tools help me out with this project.

A modern Qt6-based Graphical User Interface (GUI) tool that integrates `clang-format`,`clang-tidy`,`cppcheck`, `valgrind` and `flawfinder` to help you perform static code analysis and find errors in your C/C++ source code easily.

## 🛠 Prerequisites

First, you need to install the required dependencies and tools on your Linux environment (Debian/Ubuntu/Kali) and necessary X11 keyboard extensions for Linux UI or WSL:
```bash
sudo apt update 
sudo apt install cmake flawfinder clang-tidy cppcheck qt6-base-dev clang-format qt6-tools-dev qt6-declarative-dev build-essential valgrind libxkbcommon-dev libxkbcommon-x11-dev -y 
```

for MacOS installation with brew:
```bash
brew update 
brew install cmake qt@6 cppcheck llvm flawfinder 
```
because there leaks already on xcode-select --install
```bash
# do it after brew install on Mac, if already have skip this step  
xcode-select --install
```

## 🚀 How to Build

Second, clone this repository to your local machine:

```bash
git clone https://github.com/khanhlearnin12/codecheckerror.git
cd codecheckerror
```

Third, create a build directory and compile the project:

For linux Distro and WSL:
```bash
# for linux 
mkdir build && cd build 
cmake .. 
make 
```

for MacOs;
```bash
mkdir build && cd build
# Let CMake know where is Qt6 after brew installation 
cmake -D CMAKE_PREFIX_PATH=$(brew --prefix qt@6) ..
make
```

## 💻 How to Use

After a successful build, you can run the application directly from the terminal:

```bash
./gui

```

*(Optional)* You can also open a specific C++ file directly from the command line:

```bash
./gui path/to/your/code.cpp

```

---

*Hope you guys enjoy my tool! Feel free to open issues or contribute.* ❤️
---
![mytoolimage](mytools.png)