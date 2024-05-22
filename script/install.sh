#!/bin/bash

VERSION=v0.1
CDIR=`pwd`

INSTALLDIR=~/libian
CUDAINCLUDE=NULL
CUDALIB=BULL

if [ -d "$INSTALLDIR" ]; then
    rm -rf "$INSTALLDIR"
fi

mkdir $INSTALLDIR

PATHES=$LD_LIBRARY_PATH
PATHES+=$PATH
PATHARR=(${PATHES//:/ })

for var in ${PATHARR[@]}
do
    if echo "$var" | grep -q 'cuda'; then
        ARR=(${var//// })
        ele=${ARR[$((${#ARR[@]}-1))]}
        if [[ $ele == *lib* ]]; then
            CUDALIB=$var
        fi
        if [[ $ele == *include* ]]; then
            CUDAINCLUDE=$var
        fi
    fi
done

if [[ $CUDAINCLUDE == *NULL* ]]; then
    echo "We can not find cuda include!"
    echo "Please make sure cuda include path is in system variable"
    exit 0
fi

if [[ $CUDALIB == *NULL* ]]; then
    echo "We can not find cuda lib!"
    echo "Please make sure cuda lib path is in system variable"
    exit 0
fi

ARCHIVE=`awk '/^__ARCHIVE_BOUNDARY__/ { print NR + 1; exit 0; }' $0`

tail -n +$ARCHIVE $0 > lumos.tar.gz
tar -zxvf lumos.tar.gz
wait

rm -f lumos.tar.gz

mv ./build/include $INSTALLDIR/include
mv ./build/lib ./lib
mv ./build/lumos ./lumos
mv ./build/utils ./utils
mv ./build/makefile ./makefile

mkdir ./build/lib
mkdir ./build/obj

make -f makefile CUDAINCLUDE=$CUDAINCLUDE CUDALIB=$CUDALIB
wait

mv ./build/lib $INSTALLDIR/lib

rm -rf build
rm -rf lib
rm -rf lumos
rm -rf utils
rm -f makefile
rm -f lumos-$VERSION.run

exit 0
__ARCHIVE_BOUNDARY__