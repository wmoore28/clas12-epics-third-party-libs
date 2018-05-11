#!/bin/sh

echo "Installing software..."

mkdir -v /usr/share/CAENControlSoftware
mkdir -v /usr/share/CAENControlSoftware/plugins
mkdir -v /usr/share/CAENControlSoftware/plugins/imageformats

rm /usr/lib/libcaenhvwrapper.so 
cp -v ./lib/libcaenhvwrapper.so.5.20 /usr/lib/libcaenhvwrapper.so.5.20
ln -s /usr/lib/libcaenhvwrapper.so.5.20 /usr/lib/libcaenhvwrapper.so

rm /usr/lib/libhscaenet.so
cp -v ./lib/hscaenetlib.so.1.7 /usr/lib/hscaenetlib.so.1.7
ln -s /usr/lib/hscaenetlib.so.1.7 /usr/lib/libhscaenet.so

rm /usr/lib/libCAENVME.so
cp -v ./lib/libCAENVME.so.2.30.2 /usr/lib/libCAENVME.so.2.30.2
ln -s /usr/lib/libCAENVME.so.2.30.2 /usr/lib/libCAENVME.so

rm /usr/lib/libCAENComm.so 
cp -v ./lib/libCAENComm.so.1.02 /usr/lib/libCAENComm.so.1.02
ln -s /usr/lib/libCAENComm.so.1.02 /usr/lib/libCAENComm.so


if [ -f /usr/lib/libQtCore.so.4.7.4 ]; then
echo "libQtCore already present"
else
cp -v ./qt/libQtCore.so.4.7.4 /usr/lib/libQtCore.so.4.7.4
fi

if [ -f /usr/lib/libQtGui.so.4.7.4 ]; then
echo "libQtGui already present"
else
cp -v ./qt/libQtGui.so.4.7.4 /usr/lib/libQtGui.so.4.7.4
fi

if [ -f /usr/lib/libQtWebKit.so.4.7.4 ]; then
echo "libQtWebKit already present"
else
cp -v ./qt/libQtWebKit.so.4.7.4 /usr/lib/libQtWebKit.so.4.7.4
fi

#if [ -f /usr/lib/libQtDBus.so.4.7.4 ]; then
#echo "libQtDBus already present"
#else
#cp -v ./qt/libQtDBus.so.4.7.4 /usr/lib/libQtDBus.so.4.7.4
#fi

#if [ -f /usr/lib/libQtXml.so.4.7.4 ]; then
#echo "libQtXml.so.4.7.4 already present"
#else
#cp -v ./qt/libQtXml.so.4.7.4 /usr/lib/libQtXml.so.4.7.4
#fi

if [ -f /usr/lib//libQtNetwork.so.4.7.4 ]; then
echo "libQtNetwork already present"
else
cp -v ./qt/libQtNetwork.so.4.7.4 /usr/lib/libQtNetwork.so.4.7.4
fi

if [ -f /usr/lib/libphonon.so.4.4.0 ]; then
echo "libphonon already present"
else
cp -v ./qt/libphonon.so.4.2.0 /usr/lib/libphonon.so.4.2.0
fi

cp -fv ./qt/plugins/* /usr/share/CAENControlSoftware/plugins/
cp -fv ./qt/plugins/imageformats/* /usr/share/CAENControlSoftware/plugins/imageformats

cp -fv ./Readme.txt /usr/share/CAENControlSoftware/
cp -fv ./ReleaseNotes.txt /usr/share/CAENControlSoftware/
cp -fv ./CAEN_License_Agreement.txt /usr/share/CAENControlSoftware/
cp -fv ./bin/CAENControlSoftware /usr/share/CAENControlSoftware/
ln -fs /usr/share/CAENControlSoftware/CAENControlSoftware /usr/local/bin/CAENControlSoftware
cp -fv ./doc/CAENHVControlSw_REV2.pdf /usr/share/CAENControlSoftware/

ldconfig

echo
echo "Installation completed."
