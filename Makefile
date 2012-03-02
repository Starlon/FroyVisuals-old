# Android Makefile v0.1 (c) 2011 - Daniel Hiepler <daniel@niftylight.de>

APPNAME = FroyVisuals
ACTIVITY = FroyVisuals
TARGET = android-10

all:
	@ndk-build
	@ant clean
	@ant release

install: 
	@adb -e install -r bin/$(APPNAME).apk

install_emu:
	@adb -e install -r bin/$(APPNAME).apk

install_dev:
	@adb -d install -r bin/$(APPNAME).apk

debug:
	@ndk-build APP_OPTIM=debug NDK_DEBUG=1
	@ant clean
	@ant debug
	@adb install -r bin/$(APPNAME)-debug.apk

debug_emu:
	@ndk-build APP_OPTIM=debug NDK_DEBUG=1
	@adb -e install -r bin/$(APPNAME)-debug.apk

debug_dev:
	@ndk-build APP_OPTIM=debug NDK_DEBUG=1
	@adb -d install -r bin/$(APPNAME)-debug.apk


install_debug: 
	@adb install -r bin/$(APPNAME)-debug.apk

install_debug_emu:
	@adb -e install -r bin/$(APPNAME)-debug.apk

install_debug_dev:
	@adb -d install -r bin/$(APPNAME)-debug.apk
 
clean:
	@ndk-build clean
	@ant clean
	@rm -rf bin libs obj gen

update:
	@android update project --path . --target $(TARGET)

keygen:
	@keytool -genkey -v -keystore my.keystore -alias $(APPNAME)_key -keyalg RSA -keysize 4096 -validity 100000

sign:
	@jarsigner -keystore my.keystore bin/$(ACTIVITY)-release-unsigned.apk $(APPNAME)_key
	@zipalign -v 4 bin/$(ACTIVITY)-release-unsigned.apk bin/$(APPNAME).apk

log:
	@cd lyrical/data/data/com.starlon.froyvisuals/lib;./pull.sh; cd ../../../../../
	@/opt/arm-2011.09/bin/arm-none-linux-gnueabi-objdump -S obj/local/armeabi/libmain.so > libfroyvisuals.asm
	@adb shell logcat -d > test.log
	@./stack.py --symbols-dir=lyrical/ ./test.log 
	#This seems to suck. @./parse_stack.py ./libfroyvisuals.asm ./test.log
	@adb shell dumpsys meminfo -h > meminfo.txt

gdb:
	@ndk-gdb --start --force --verbose

mem:
	@adb shell setprop dalvik.vm.checkjni true
	@adb shell setprop libc.debug.malloc 10
	@adb shell setprop dalvik.vm.jniopts forcecopy
	@adb shell start
	@adb shell stop

start:
	@adb shell am start -n com.starlon.froyvisuals/.FroyVisuals

valgrind:
	@adb shell setprop wrap.com.starlon.froyvisuals "logwrapper valgrind"

redirect:
	@adb shell stop
	@adb shell setprop log.redirect-stdio true
	@adb shell start

profile:
	@./gprof.sh


