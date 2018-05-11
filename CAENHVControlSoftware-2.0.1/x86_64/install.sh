#!/bin/sh

echo "Installing software..."

mkdir -v /usr/share/CAENControlSoftware
mkdir -v /usr/share/CAENControlSoftware/plugins
mkdir -v /usr/share/CAENControlSoftware/plugins/imageformats

rm /usr/lib64/libcaenhvwrapper.so 
cp -v ./lib/x64/libcaenhvwrapper.so.5.21 /usr/lib64/libcaenhvwrapper.so.5.21
ln -s /usr/lib64/libcaenhvwrapper.so.5.21 /usr/lib64/libcaenhvwrapper.so

rm /usr/lib64/libhscaenet.so
cp -v ./lib/x64/hscaenetlib.so.1.7 /usr/lib64/hscaenetlib.so.1.7
ln -s /usr/lib64/hscaenetlib.so.1.7 /usr/lib64/libhscaenet.so

rm /usr/lib64/libCAENVME.so
cp -v ./lib/x64/libCAENVME.so.2.30.2 /usr/lib64/libCAENVME.so.2.30.2
ln -s /usr/lib64/libCAENVME.so.2.30.2 /usr/lib64/libCAENVME.so

rm /usr/lib64/libCAENComm.so 
cp -v ./lib/x64/libCAENComm.so.1.02 /usr/lib64/libCAENComm.so.1.02
ln -s /usr/lib64/libCAENComm.so.1.02 /usr/lib64/libCAENComm.so


if [ -f /usr/lib64/libQtCore.so.4.7.4 ]; then
echo "libQtCore already present"
else
cp -v ./qt/x64/libQtCore.so.4.7.4 /usr/lib64/libQtCore.so.4.7.4
fi

if [ -f /usr/lib64/libQtGui.so.4.7.4 ]; then
echo "libQtGui already present"
else
cp -v ./qt/x64/libQtGui.so.4.7.4 /usr/lib64/libQtGui.so.4.7.4
fi

if [ -f /usr/lib64/libQtWebKit.so.4.7.4 ]; then
echo "libQtWebKit already present"
else
cp -v ./qt/x64/libQtWebKit.so.4.7.4 /usr/lib64/libQtWebKit.so.4.7.4
fi

if [ -f /usr/lib64/libQtDBus.so.4.7.4 ]; then
echo "libQtDBus already present"
else
cp -v ./qt/x64/libQtDBus.so.4.7.4 /usr/lib64/libQtDBus.so.4.7.4
fi

if [ -f /usr/lib64/libQtXml.so.4.7.4 ]; then
echo "libQtXml already present"
else
cp -v ./qt/x64/libQtXml.so.4.7.4 /usr/lib64/libQtXml.so.4.7.4
fi

if [ -f /usr/lib64/libQtNetwork.so.4.7.4 ]; then
echo "libQtNetwork already present"
else
cp -v ./qt/x64/libQtNetwork.so.4.7.4 /usr/lib64/libQtNetwork.so.4.7.4
fi

if [ -f /usr/lib64/libphonon.so.4.2.0 ]; then
echo "libphonon already present"
else
cp -v ./qt/x64/libphonon.so.4.2.0 /usr/lib64/libphonon.so.4.2.0
fi

cp -fv ./qt/x64/plugins/* /usr/share/CAENControlSoftware/plugins/
cp -fv ./qt/x64/plugins/imageformats/* /usr/share/CAENControlSoftware/plugins/imageformats

cp -fv ./Readme.txt /usr/share/CAENControlSoftware/
cp -fv ./ReleaseNotes.txt /usr/share/CAENControlSoftware/
cp -fv ./CAEN_License_Agreement.txt /usr/share/CAENControlSoftware/
cp -fv ./bin/x64/CAENControlSoftware /usr/share/CAENControlSoftware/
ln -fs /usr/share/CAENControlSoftware/CAENControlSoftware /usr/local/bin/CAENControlSoftware
chmod +x /usr/share/CAENControlSoftware
cp -fv ./doc/CAENHVControlSw_REV2.pdf /usr/share/CAENControlSoftware/

ldconfig

echo
echo "Installation completed."
