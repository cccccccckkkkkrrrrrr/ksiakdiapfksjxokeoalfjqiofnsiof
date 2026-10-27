#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include "foto.h"
#include "oret.h"

JNIEXPORT jbyteArray JNICALL
Java_com_if831c2cf3041abe0_revamped_by_pbcat_DriverInstall_gFoto(JNIEnv* env, jclass clazz) {
    jbyteArray arr = (*env)->NewByteArray(env, foto_png_len);
    (*env)->SetByteArrayRegion(env, arr, 0, foto_png_len, (const jbyte*)foto_png);
    return arr;
}

JNIEXPORT jbyteArray JNICALL
Java_com_if831c2cf3041abe0_revamped_by_pbcat_DriverInstall_gOret(JNIEnv* env, jclass clazz) {
    jbyteArray arr = (*env)->NewByteArray(env, oret_mp3_len);
    (*env)->SetByteArrayRegion(env, arr, 0, oret_mp3_len, (const jbyte*)oret_mp3);
    return arr;
}
