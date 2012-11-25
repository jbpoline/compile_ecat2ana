#!/bin/bash 

tp_path=${PWD}
cd libtpcmisc

echo ${tp_path}
make -e

cd ../libtpcimgio
export INCLUDE="-I $tp_path/libtpcmisc/include -I $tp_path/libtpcimgio/include"
echo $INCLUDE
export LDFLAGS="-L ${tp_path}/libtpcmisc -L ${tp_path}/libtpcimgio"
echo $LDFLAGS
make -e 

cd ../libtpcimgp
export INCLUDE="$INCLUDE -I $tp_path/libtpcimgp/include"
export LDFLAGS="$LDFLAGS -L $tp_path/libtpcimgp"
make -e

cd ../ecat2ana
make -e


