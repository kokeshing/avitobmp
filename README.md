# avitobmp

aviファイルの任意の区間を連番bmpに

## Description

なんか課題で画像作成してそれからgif作れって言われたのでこれ作りました。
640x360のaviにしか対応してません（怠慢）

## Features

- 標準ライブラリしか使ってない

## Unfeatures

- 640x360のavi限定
- 出力1枚目の色がおかしい（既知の不具合）
- できたbmpファイルたぶんヘッダが変(ちゃんと見れはする)
- 数々の怠慢設計

## Usage

	$ ./a.out
	aviファイルの場所を入力
	hogefoge
	gifにするはじめの動画時間(秒)を入力
	hoge_second
	gifにする終わりの動画時間(秒)を入力
	fogehoge_second

## Installation

    $ git clone  https://github.com/kokeshing/avitobmp.git
    $ cd avitobmp
    $ gcc main.c avi_lib.c bmp_lib.c

## 💩Author

[@kokeshing_](https://twitter.com/kokeshing_)