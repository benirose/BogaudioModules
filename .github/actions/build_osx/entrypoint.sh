#!/bin/sh

set -eu

echo
echo
echo
echo
echo
echo Actual build starts now....
echo
echo
echo
echo
echo

export RACK_DIR=${GITHUB_WORKSPACE}/Rack-SDK
export RACK_USER_DIR=${GITHUB_WORKSPACE}

export CC=x86_64-apple-darwin15-clang
export CXX=x86_64-apple-darwin15-clang++
export STRIP=x86_64-apple-darwin15-strip

curl -L https://vcvrack.com/downloads/Rack-SDK-${RACK_SDK_VERSION}.zip -o rack-sdk.zip
unzip -q -o rack-sdk.zip
rm rack-sdk.zip

make dist
