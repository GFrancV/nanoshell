# nanoShell

A small project in C to replicate a Linux terminal with the possibility of output addressing and signal processing.

This project is only available in linux enviroment.

![Preview](https://raw.githubusercontent.com/GFrancV/nanoshell/main/preview.PNG)

## Installation:

```bash
# Clone the repo.
git clone https://github.com/GFrancV/nanoshell.git

# Move to the directory
cd nanoshell

# Compile the project
make
```

Make sure you have ```~/usr/bin/gcc``` installed and updated to the latest version.

```bash
# Install GCC
sudo apt-get install gcc

# Update all repositories
sudo apt-get update
```

## Usage:

Table of the option available for nanoShell.

|Options |Description|
|:---:|:---|
|-m {int}|Maximum of comands to be executed for nanoShell. **This option is not compatible with other options**. |
|-f {file} |Execute the comands define in a .txt file. **This option is not compatible with other options**. |
|-s |When nanoShell start create a new file **signal.txt**, in this file are the necessary comands to send signals to the nanoShell.|
|-h |See the man of nanoShell.|

## Clean

For clean all the dependences (`*.o` files) and the executable file of nanoShell.

```bash
make clean
```


