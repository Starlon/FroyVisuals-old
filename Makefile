# Android Makefile v0.1 (c) 2011 - Daniel Hiepler <daniel@niftylight.de>

APPNAME = FroyVisuals
ACTIVITY = FroyVisuals

all:
	@ndk-build
	@ant clean
	@ant release
install:
	@adb install -r bin/FroyVisuals.apk

debug:
	@ndk-build APP_OPTIM=debug NDK_DEBUG=1
	@ant clean
	@ant debug

debuginstall:
	@adb install -r bin/FroyVisuals-debug.apk

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

log:
	@/opt/arm-2011.09/bin/arm-none-linux-gnueabi-objdump -S obj/local/armeabi/libfroyvisuals.so > libfroyvisuals.asm
	@adb shell logcat -d > test.log
	@./stack.py --symbols-dir=lyrical/ ./test.log 
	@./parse_stack.py ./libfroyvisuals.asm ./test.log
	@adb shell dumpsys meminfo -h > meminfo.txt

gdb:
	@ndk-gdb --start --force --verbose
	@/opt/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-gdb -x obj/local/armeabi-v7a/gdb.setup -e obj/local/armeabi-v7a/app_process 

