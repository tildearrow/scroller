#!/bin/sh
echo "Creating dmg file..."
mkdir scroller
cp ../build/scroller scroller
cp ../README.md scroller
cp ../LICENSE scroller
hdiutil create scroller.dmg -volname "scroller" -srcfolder scroller
