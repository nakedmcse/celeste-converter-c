# Celeste Converter C

A C implementation of a tool for converting between Celeste game assets (DATA format) and PNG images.

## Overview

This tool allows you to convert between Celeste's proprietary DATA format and standard PNG images. It's a C port of the original [Rust-based celeste-converter](https://github.com/borogk/celeste-converter).

## Features

- Convert DATA files to PNG images
- Convert PNG images back to DATA files
- Preserve alpha channel information
- Run-length encoding (RLE) compression support

## Usage

```
celeste-converter [options] [command] <from-directory-or-file> <to-directory-or-file>
```

Available commands:
- `data2png`: Convert DATA files to PNG images
- `png2data`: Convert PNG images to DATA files

Options:
- `-h`: Show help text
- `-v`: Enable verbose logging

### Examples

```sh
# Convert all .data files in the "assets" directory to PNG files in the "output" directory
celeste-converter data2png ./assets ./output

# Convert all PNG files back to DATA format
celeste-converter png2data ./modified_assets ./output

# Convert multi.data to multi-mod.png
celeste-converter data2png multi.data multi-mod.png

# Convert with verbose logging
celeste-converter -v data2png ./assets ./output
```

## Building from Source

```sh
# Clone the repository
git clone https://github.com/nakedmcse/celeste-converter-c.git
cd celeste-converter-c

# Build the project
make all
```
