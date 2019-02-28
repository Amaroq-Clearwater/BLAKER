# BLAKER

BLAKER is a Windows application for transferring files via papers, images, or movies.

## How to use BLAKER Papers

1. Load some files into BLAKER.
2. Print as physical papers by BLAKER.
3. Make the papers a PDF file anyway.
4. Scan that PDF file by BLAKER at another PC.
5. The original file(s) will be restored.

## How to use BLAKER Images

1. Load some files into BLAKER.
2. Screen the images by BLAKER and take picture files.
3. Scan those image files by BLAKER at another PC.
4. The original file(s) will be restored.

## How to use BLAKER Movie

1. Load some files into BLAKER.
2. Screen the movie by BLAKER and take a movie file.
3. Scan that movie file by BLAKER at another PC.
4. The original file(s) will be restored.

## Restrictions

- Up to 99 pages can be printed.
- The amount of printable data after compression is 256 KiB.
- The amount of screenable data after compression is 1.5 MiB.

## How to build?

You need `ZLIB`, `BZip2`, `LibLZMA`, `PNG`, `Iconv` and `OpenCV` libraries.

If you are using MSYS2, please install them by `pacman`. Otherwise, you have to install them manually.

```bash
$ git clone --recursive https://github.com/katahiromz/BLAKER.git
$ cd BLAKER
$ cmake -G "MSYS Makefiles" .
$ make -j2
```
