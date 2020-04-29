# one-north-bank
A minimalist program for One North Bank, a hypothetical bank.

### Table of Contents

### TODO
- encrypt/decrypt after getting/setting sstream in ral
- sanitize user input
- create logout functionalit in bank class
- clean up documentation

### Quickstart:
1. call make (builds)
2. call make run

### makefile commands
- build: create directory if needed & compile the project
- rebuild: delete the executable & call build command
- run: call the executable
- restart: call rebuild then call run
- reboot: call destroy then build then run
- clean: remove executable
- destroy: remove executable & stored account info
- ./OneNorthBank: runs the executable
- directory: makes the directory for the executable

### Architecture
utility namespace: helper functions

ral namespace: random access library
- abstract record class
- file class: takes record pointers
- creates/reads a .raf file (extension is customizable) with up to 100 records

bank class:
- uses ral::record for bank::account
- has an instance of ral::file
- stores a current user
- logic that edits an account is in bank::account to keep it centralized

main:
- loginRequested: asks user if they want to create account or login
- promptMenu: after logging in, asks user what they'd like to do

### file types
- .h: header files with minimal code/includes
- .cpp: source files
- .tpp: header files with template function implementations
- .raf: random access file created by ral

### source code structure
- bin: where makefile stores the executable (not stored in the repo)
- include: header files
- src: source files
