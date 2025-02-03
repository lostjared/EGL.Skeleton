cmake_minimum_required(VERSION 3.10)
project(AndroidBlocks)
if(NOT DEFINED ANDROID_NDK)
    set(ANDROID_NDK $ENV{ANDROID_NDK})
endif()
if(NOT ANDROID_NDK)
    message(FATAL_ERROR "ANDROID_NDK variable not set. Please set it to your Android NDK path.")
endif()

set(APP_GLUE_DIR ${ANDROID_NDK}/sources/android/native_app_glue)
add_library(native_app_glue STATIC ${APP_GLUE_DIR}/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC ${APP_GLUE_DIR})
add_library(native-lib SHARED egl_droid.cpp droid_main.cpp)
find_library(log-lib log)
find_library(android-lib android)
find_library(egl-lib EGL)
find_library(gles3-lib GLESv3)
target_link_libraries(native-lib
        native_app_glue
        ${log-lib}
        ${android-lib}
        ${egl-lib}
        ${gles3-lib}
)