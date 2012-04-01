export ANDROID_CMAKE=../android-cmake
export NDK=/opt/android-ndk-r7b
export ANDROID_SDK=/opt/android-sdk-linux

export ANDTOOLCHAIN=$ANDROID_CMAKE/toolchain/android.toolchain.cmake

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDTOOLCHAIN ../
