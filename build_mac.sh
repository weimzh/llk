#!/bin/sh
qmake -macx
make
mkdir llk.app/Contents/Resources
cp -r wav llk.app/Contents/Resources
cp -r icons llk.app/Contents/Resources
cp pics/* llk.app/Contents/Resources
cp llk.icns llk.app/Contents/Resources
cp -f Info.plist llk.app/Contents
cp *.qm llk.app/Contents/Resources
mkdir llk.app/Contents/Resources/zh_CN.lproj
cp InfoPlist.strings llk.app/Contents/Resources/zh_CN.lproj
mkdir llk.app/Contents/Resources/English.lproj
cp InfoPlist.English llk.app/Contents/Resources/English.lproj
mkdir llk.app/Contents/Frameworks
/bin/cp $QTDIR/lib/libqt-mt.3.dylib llk.app/Contents/Frameworks
install_name_tool -change libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib llk.app/Contents/MacOS/llk
