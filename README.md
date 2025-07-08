c-amxx-autocompile

A C utility that wraps the AMX Mod X compiler (compile.exe) to automate plugin compilation and deployment.
Overview

Features

    Automated Compilation: Runs compile.exe with simulated Enter key press
    Smart Output Parsing: Identifies filenames, success messages, and errors from compiler output
    File Management: Automatically copies compiled .amxx files from ./compiled/ to ./../plugins/
    Error Reporting: Displays compilation errors with clear formatting
    Safe Operation: Includes buffer overflow protection and proper error handling

Directory Structure

amxmodx/
├── plugins/              # Final destination for plugins
│   └── *.amxx           # Deployed plugin files
└── scripting/           # Development directory
    ├── autocompile.exe  # This utility
    ├── compile.exe      # AMX Mod X compiler
    ├── main.c           # Source code
    ├── Makefile         # Build configuration
    ├── compiled/        # Temporary directory for compiled files
    │   └── *.amxx      # Generated plugin files
    └── *.sma           # Plugin source files

Usage
Basic Usage

./autocompile.exe


Compilation

Clone the repository:

git clone https://github.com/username/c-amxx-autocompile.git
cd c-amxx-autocompile

Using the provided Makefile:

make
