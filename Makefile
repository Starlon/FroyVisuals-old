# Android Makefile v0.1 (c) 2011 - Daniel Hiepler <daniel@niftylight.de>

APPNAME = FroyVisuals
ACTIVITY = FroyVisuals

all:
	@ndk-build
	@ant clean
	@ant release
install:
	@adb -d install -r bin/FroyVisuals.apk
	@make upload

debug:
	@ndk-build APP_OPTIM=debug
	@ant clean
	@ant debug

debuginstall:
	@adb -d install -r bin/FroyVisuals-debug.apk
	@make upload

upload:
	@adb -d push libs/armeabi/libinput_alsa.so /data/local/libvisual/plugins/input/
	@adb -d push libs/armeabi/libactor_lv_scope.so /data/local/libvisual/plugins/actor/

clean:
	@ndk-build clean
	@ant clean

update:
	@android update project --path . --target android-10

keygen:
	@keytool -genkey -v -keystore my.keystore -alias $(APPNAME)_key -keyalg RSA -keysize 4096 -validity 100000

sign:
	@jarsigner -keystore my.keystore bin/$(ACTIVITY)-release-unsigned.apk $(APPNAME)_key
	@zipalign -v 4 bin/$(ACTIVITY)-release-unsigned.apk bin/$(APPNAME).apk

init:
	@adb -d shell mkdir /data/local/libvisual
	@adb -d shell mkdir /data/local/libvisual/plugins
	@adb -d shell mkdir /data/local/libvisual/plugins/input
	@adb -d shell mkdir /data/local/libvisual/plugins/actor
	@adb -d shell mkdir /data/local/libvisual/plugins/morph
log:
	@/opt/arm-2011.09/bin/arm-none-linux-gnueabi-objdump -S obj/local/armeabi/libfroyvisuals.so > libfroyvisuals.asm
	@adb -d shell logcat -d -f /mnt/sdcard/test.log
	@adb -d pull /mnt/sdcard/test.log
	#@./stack.py --symbols-dir=lyrical/ ./test.log 
	@./parse_stack.py ./libfroyvisuals.asm ./test.log
	@adb -d shell dumpsys meminfo -h > meminfo.txt
	@tail ./test.log
