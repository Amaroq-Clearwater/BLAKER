# BLAKER

BLAKER (ブレイカー) は、紙、画像、または動画を用いてデータファイルを転送するためのWindowsアプリケーションです。

## BLAKER Papers の使い方

1. いくつかファイルをBLAKERに読み込む。
2. BLAKERで物理的な紙に印刷する。
3. とにかくそれらの紙をPDFファイルにする。
4. 別のパソコンでそのPDFファイルをBLAKERでスキャンする。
5. 元のファイルが復元される。

## BLAKER Images の使い方

1. いくつかファイルをBLAKERに読み込む。
2. BLAKERで画像を上映し、その画像を撮影する。
3. 別のパソコンでそれらの画像ファイルをBLAKERでスキャンする。
4. 元のファイルが復元される。

## BLAKER Movie の使い方

1. いくつかファイルをBLAKERに読み込む。
2. BLAKERで動画を上映し、その動画を撮影する。
3. 別のパソコンでその動画ファイルをBLAKERでスキャンする。
4. 元のファイルが復元される。

## 制限

- 印刷可能なページ数は99ページまで。
- 圧縮後の印刷可能なデータ量は256KiBまで。
- 圧縮後の上映可能なデータ量は1.5MiBまで。

## ビルド方法は?

`ZLIB`, `BZip2`, `LibLZMA`, `PNG`, `Iconv` and `OpenCV` ライブラリーが必要です。

MSYS2を使っているのであれば、それらを`pacman`でインストールして下さい。
さもなければ、それらを手動でインストールする必要があります。

```bash
$ git clone --recursive https://github.com/katahiromz/BLAKER.git
$ cd BLAKER
$ cmake -G "MSYS Makefiles" .
$ make -j2
```
