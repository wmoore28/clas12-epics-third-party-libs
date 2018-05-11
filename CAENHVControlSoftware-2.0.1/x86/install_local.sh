#!/bin/sh

echo "Installing software..."

cd lib

rm -rf libcaenhvwrapper.so 
ln -s libcaenhvwrapper.so.5.20 libcaenhvwrapper.so

rm -rf libhscaenet.so
ln -s hscaenetlib.so.1.7 libhscaenet.so

echo
echo "Installation completed."
