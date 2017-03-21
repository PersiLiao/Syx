#!/bin/sh
phpize && ./configure --enable-debug --enable-syx-debug && make && make install
rm -rf .deps
rm -rf cmake-build-debug
rm -rf Makefile
rm -rf Makefile.fragments
rm -rf Makefile.global
rm -rf Makefile.objects
rm -rf acinclude.m4
rm -rf aclocal.m4
rm -rf autom4te.cache
rm -rf build
rm -rf config.guess
rm -rf config.h
rm -rf config.h.in
rm -rf config.nice
rm -rf config.status
rm -rf config.sub
rm -rf configure
rm -rf configure.in
rm -rf install-sh
rm -rf libtool
rm -rf ltmain.sh
rm -rf missing
rm -rf mkinstalldirs
rm -rf run-tests.php
