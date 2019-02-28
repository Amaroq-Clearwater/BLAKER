# <img src="B.png" alt="" width="32" height="32" /> BLAKER

BLAKER is a Windows application for transferring files via papers, images, or movies.

This is not a joke program but solution of realistic business issues.

Download: https://katahiromz.web.fc2.com/blaker/en

## How to use BLAKER Papers

1. Load some files into BLAKER.
2. Print as physical papers by BLAKER.
3. Make the papers a PDF file anyway.
4. Scan that PDF file by BLAKER at another PC.
5. The original file(s) will be restored.

We don't provide TWAIN interface. Please convert papers to a PDF file by another software.

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

## How much data size can be printed on one sheet?

In default 60 DPI, it's about 20 KiB (after compression) in an A4 sheet (this is not a joke).

You can choose the higher dot density, but it might cause reading failure.

## Restrictions

- Up to 99 pages can be printed.
- The amount of printable data after compression is 256 KiB.
- The amount of screenable data after compression is 1.5 MiB.

Do not waste papers. You can virtually print to a PDF file by using the PDF printer.

## How to build?

You need `ZLIB`, `BZip2`, `LibLZMA`, `PNG`, `Iconv` and `OpenCV` libraries.

If you are using MSYS2, please install them by `pacman`. Otherwise, you have to install them manually.

```bash
$ git clone --recursive https://github.com/katahiromz/BLAKER.git
$ cd BLAKER
$ cmake -G "MSYS Makefiles" .
$ make -j2
```

## Contact Us

katayama.hirofumi.mz@gmail.com
