## はじめに
upxコマンドを使ったバイナリサイズの削減について調査する

## まとめ
- 実行ファイルを圧縮できることを確認した
- 共有ライブラリも圧縮できることを確認した
- dlopenを使って共有ライブラリ内の関数を呼び出すことも可能だと確認した
- クロスコンパイルしたバイナリも圧縮可能だと確認した

## 動作確認
```bash
gcc -fPIC -shared -o libadd.so add.c
gcc -o main main.c -L. -ladd
gcc -o main_with_dlopen main_with_dlopen.c -ldl
LD_LIBRARY_PATH=. ./main
LD_LIBRARY_PATH=. ./main_with_dlopen
```

## 期待出力
```bash
main.c: Result of 5 + 3 = 8
main_with_dlopen.c: Result of 5 + 3 = 8
```

## upxでバイナリサイズを削減

upxコマンドで実行ファイルのバイナリサイズを削減する。

```bash
# upxの前にstripで不要な情報を削除しておく
strip main main_with_dlopen libadd.so
# upxで自己解凍ファイルに圧縮する
upx main main_with_dlopen libadd.so
```
```bash
                       Ultimate Packer for eXecutables
                          Copyright (C) 1996 - 2024
UPX 4.2.2       Markus Oberhumer, Laszlo Molnar & John Reiser    Jan 3rd 2024

        File size         Ratio      Format      Name
   --------------------   ------   -----------   -----------
     15984 ->      5928   37.09%   linux/amd64   main
     16200 ->      6452   39.83%   linux/amd64   main_with_dlopen
     15096 ->      8236   54.56%   linux/amd64   libadd.so
   --------------------   ------   -----------   -----------
     47280 ->     20616   43.60%                 [ 3 files ]

Packed 3 files.
```

## upxで圧縮した実行ファイルで再実行

```bash
LD_LIBRARY_PATH=. ./main
LD_LIBRARY_PATH=. ./main_with_dlopen
```

## 期待動作

```bash
main.c: Result of 5 + 3 = 8
main_with_dlopen.c: Result of 5 + 3 = 8
```

## クロスコンパイル

aarch64 環境向けのビルドツールと実行確認用に QEMU を用意する

```bash
sudo apt install -y g++-aarch64-linux-gnu
sudo apt install -y qemu-system-aarch64
```

クロスコンパイルしたプログラムの実行結果を確認する

```bash
aarch64-linux-gnu-gcc -fPIC -shared -o libadd_aarch64.so add.c
aarch64-linux-gnu-gcc -o main_aarch64 main.c -L. -ladd_aarch64
aarch64-linux-gnu-gcc -o main_with_dlopen_aarch64 main_with_dlopen.c -ldl
file main_aarch64 main_with_dlopen_aarch64 libadd_aarch64.so
LD_LIBRARY_PATH=. qemu-aarch64 -L /usr/aarch64-linux-gnu ./main_aarch64
LD_LIBRARY_PATH=. qemu-aarch64 -L /usr/aarch64-linux-gnu ./main_with_dlopen_aarch64
```


## 期待出力
```
main_aarch64:             ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), statically linked, no section header
main_with_dlopen_aarch64: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), statically linked, no section header
libadd_aarch64.so:        ELF 64-bit LSB shared object, ARM aarch64, version 1 (SYSV), dynamically linked, BuildID[sha1]=da4bcc02b42294218d7f60c0a01f2d5956bf5349, stripped
main.c: Result of 5 + 3 = 8
main_with_dlopen.c: Result of 5 + 3 = 8
```

# upx
```bash
aarch64-linux-gnu-strip main_aarch64 main_with_dlopen_aarch64  libadd_aarch64.so
upx main_aarch64 main_with_dlopen_aarch64 libadd_aarch64.so
LD_LIBRARY_PATH=. qemu-aarch64 -L /usr/aarch64-linux-gnu ./main_aarch64
LD_LIBRARY_PATH=. qemu-aarch64 -L /usr/aarch64-linux-gnu ./main_with_dlopen_aarch64
```

## 期待出力
upxで圧縮する際に圧縮率が悪い場合には圧縮エラー(NotCompressibleException)で圧縮しない場合がある

```bash
main.c: Result of 5 + 3 = 8
main_with_dlopen.c: Result of 5 + 3 = 8
```