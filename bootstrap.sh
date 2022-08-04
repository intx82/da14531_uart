#!/bin/sh

if [ ! -d "DA145xx_SDK" ]; then
	unzip -o SDK_6.0.14.1114.zip
	ln -s "DA145xx_SDK/6.0.14.1114/sdk" sdk
        cp -r "DA145xx_SDK/6.0.14.1114/third_party" "sdk/"
fi