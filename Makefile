# Android Makefile v0.1 (c) 2011 - Daniel Hiepler <daniel@niftylight.de>


APPNAME = StarVisuals
ACTIVITY = StarVisuals
TARGET = android-15

all:
	@ndk-build
	@ant clean
	@ant release

install: 
	@adb install -r bin/$(APPNAME).apk

install_emu:
	@adb -e install -r bin/$(APPNAME).apk

install_dev:
	@adb -d install -r bin/$(APPNAME)-debug.apk

debug:
	@ndk-build APP_OPTIM=debug NDK_DEBUG=1
	@ant clean
	@ant debug

debug_emu:
	@ndk-build APP_OPTIM=debug NDK_DEBUG=1
	@adb -e install -r bin/$(APPNAME)-debug.apk

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
	@adb pull /data/data/com.starlon.starvisuals/lib/ lyrical/data/data/com.starlon.starvisuals/
	@/opt/arm-2011.09/bin/arm-none-linux-gnueabi-objdump -S obj/local/armeabi/libvisual.so > libstarvisuals.asm
	@adb shell logcat -d > test.log
	@./tools/stack.py --symbols-dir=./lyrical/ ./test.log 
	#@./tools/parse_stack.py ./libstarvisuals.asm ./test.log
	@adb shell dumpsys meminfo -h > meminfo.txt

gdb:
	@ndk-gdb --start --force --verbose

mem:
	@adb shell setprop dalvik.vm.checkjni true
	@adb shell setprop libc.debug.malloc 10
	@adb shell setprop dalvik.vm.jniopts forcecopy
	@adb shell stop
	@adb shell start

start:
	@adb shell am start -n com.starlon.starvisuals/.StarVisuals

start_dev:
	@adb -d shell am start -n com.starlon.starvisuals/.StarVisuals

start_emu:
	@adb -e shell am start -n com.starlon.starvisuals/.StarVisuals

valgrind:
	@adb shell setprop wrap.com.starlon.starvisuals "logwrapper valgrind"

redirect:
	@adb shell stop
	@adb shell setprop log.redirect-stdio true
	@adb shell start

profile:
	@./tools/gprof.sh


