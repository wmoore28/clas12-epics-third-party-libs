#!/bin/sh

mkdir -v /usr/share/CAENControlSofware
mkdir -v /usr/share/CAENControlSofware/plugins
mkdir -v /usr/share/CAENControlSofware/plugins/imageformats

cp -v ./lib/libcaenhvwrapper.so.5.00 /usr/lib/libcaenhvwrapper.so.5.00
ln -fs /usr/lib/libcaenhvwrapper.so.5.00 /usr/lib/libcaenhvwrapper.so
cp -v ./lib/hscaenetlib.so.1.7 /usr/lib/hscaenetlib.so.1.7
ln -s /usr/lib/hscaenetlib.so.1.7 /usr/lib/libhscaenet.so

cp -v ./qt/libQtCore.so.4.7.4 /usr/lib/libQtCore.so.4.7.4
ln -s /usr/lib/libQtCore.so.4.7.4 /usr/lib/libQtCore.so.4
ln -s /usr/lib/libQtCore.so.4 /usr/lib/libQtCore.so
cp -v ./qt/libQtGui.so.4.7.4 /usr/lib/libQtGui.so.4.7.4
ln -s /usr/lib/libQtGui.so.4.7.4 /usr/lib/libQtGui.so.4
ln -s /usr/lib/libQtGui.so.4 /usr/lib/libQtGui.so
cp -v ./qt/libQtWebKit.so.4.7.4 /usr/lib/libQtWebKit.so.4.7.4
ln -s /usr/lib/libQtWebKit.so.4.7.4 /usr/lib/libQtWebKit.so.4
ln -s /usr/lib/libQtWebKit.so.4 /usr/lib/libQtWebKit.so
cp -v ./qt/libQtDBus.so.4.7.4 /usr/lib/libQtDBus.so.4.7.4
ln -s /usr/lib/libQtDBus.so.4.7.4 /usr/lib/libQtDBus.so.4
ln -s /usr/lib/libQtDBus.so.4 /usr/lib/libQtDBus.so
cp -v ./qt/libQtXml.so.4.7.4 /usr/lib/libQtXml.so.4.7.4
ln -s /usr/lib/libQtXml.so.4.7.4 /usr/lib/libQtXml.so.4
ln -s /usr/lib/libQtXml.so.4 /usr/lib/libQtXml.so
cp -v ./qt/libQtNetwork.so.4.7.4 /usr/lib/libQtNetwork.so.4.7.4
ln -s /usr/lib/libQtNetwork.so.4.7.4 /usr/lib/libQtNetwork.so.4
ln -s /usr/lib/libQtNetwork.so.4 /usr/lib/libQtNetwork.so
cp -v ./qt/libphonon.so.4.4.0 /usr/lib/libphonon.so.4.4.0
ln -s /usr/lib/libphonon.so.4.4.0 /usr/lib/libphonon.so

cp -v ./qt/plugins/* /usr/share/CAENControlSofware/plugins/
cp -v ./qt/plugins/imageformats/* /usr/share/CAENControlSofware/plugins/imageformats

cp -fv ./Readme.txt /usr/share/CAENControlSofware/
cp -fv ./ReleaseNotes.txt /usr/share/CAENControlSofware/
cp -fv ./bin/CAENControlSoftware /usr/share/CAENControlSofware/CAENControlSoftware
ln -fs /usr/share/CAENControlSofware/CAENControlSoftware /usr/local/bin/CAENControlSoftware

ldconfig
