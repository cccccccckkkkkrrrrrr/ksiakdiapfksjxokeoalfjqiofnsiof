#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char *B16 = "0123456789ABCDEF";
static const char *B32 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static const char *B36 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *B58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const char *B62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const char *B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char *B85 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
static const char *B91 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&()*+,./:;<=>?@[]^_`{|}~\"";

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase16(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 2 + 1);
    for (int i = 0; i < n; i++) {
        o[i * 2] = B16[(p[i] >> 4) & 0x0F];
        o[i * 2 + 1] = B16[p[i] & 0x0F];
    }
    o[n * 2] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase2(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 8 + 1);
    for (int i = 0; i < n; i++) {
        for (int j = 7; j >= 0; j--) {
            o[i * 8 + (7 - j)] = ((p[i] >> j) & 1) ? '1' : '0';
        }
    }
    o[n * 8] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase8(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 3 + 1);
    for (int i = 0; i < n; i++) {
        o[i * 3] = '0' + ((p[i] >> 6) & 7);
        o[i * 3 + 1] = '0' + ((p[i] >> 3) & 7);
        o[i * 3 + 2] = '0' + (p[i] & 7);
    }
    o[n * 3] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase10(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    unsigned long long v = 0;
    for (int i = 0; i < n; i++) v = v * 256 + (unsigned char)p[i];
    char o[32];
    sprintf(o, "%llu", v);
    jstring r = (*e)->NewStringUTF(e, o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase32(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc((n * 8 + 4) / 5 + 1);
    int buf = 0, bits = 0, j = 0;
    for (int i = 0; i < n; i++) {
        buf = (buf << 8) | (unsigned char)p[i];
        bits += 8;
        while (bits >= 5) {
            o[j++] = B32[(buf >> (bits - 5)) & 0x1F];
            bits -= 5;
        }
    }
    if (bits > 0) o[j++] = B32[(buf << (5 - bits)) & 0x1F];
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

static jstring ullToStr(JNIEnv *e, unsigned long long v, const char *chars, int base) {
    char o[128];
    int pos = 0;
    if (v == 0) o[pos++] = chars[0];
    while (v > 0) { o[pos++] = chars[v % base]; v /= base; }
    o[pos] = 0;
    for (int i = 0; i < pos / 2; i++) {
        char tmp = o[i]; o[i] = o[pos - 1 - i]; o[pos - 1 - i] = tmp;
    }
    return (*e)->NewStringUTF(e, o);
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase36(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    unsigned long long v = 0;
    for (int i = 0; i < n; i++) v = v * 256 + (unsigned char)p[i];
    jstring r = ullToStr(e, v, B36, 36);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase58(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    unsigned long long v = 0;
    for (int i = 0; i < n; i++) v = v * 256 + (unsigned char)p[i];
    jstring r = ullToStr(e, v, B58, 58);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase62(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    unsigned long long v = 0;
    for (int i = 0; i < n; i++) v = v * 256 + (unsigned char)p[i];
    jstring r = ullToStr(e, v, B62, 62);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase64(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    int outLen = 4 * ((n + 2) / 3);
    char *o = (char*)malloc(outLen + 1);
    int i, j;
    for (i = 0, j = 0; i < n;) {
        unsigned int a = i < n ? (unsigned char)p[i++] : 0;
        unsigned int b = i < n ? (unsigned char)p[i++] : 0;
        unsigned int c = i < n ? (unsigned char)p[i++] : 0;
        unsigned int tr = (a << 16) | (b << 8) | c;
        o[j++] = B64[(tr >> 18) & 0x3F];
        o[j++] = B64[(tr >> 12) & 0x3F];
        o[j++] = B64[(tr >> 6) & 0x3F];
        o[j++] = B64[tr & 0x3F];
    }
    for (int k = 0; k < (3 - n % 3) % 3; k++) o[outLen - 1 - k] = '=';
    o[outLen] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase85(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc((n * 5 / 4) + 10);
    int i = 0, j = 0;
    while (i < n) {
        unsigned long long ch = 0;
        int cnt = (n - i < 4) ? (n - i) : 4;
        for (int k = 0; k < 4; k++) ch = (ch << 8) | (k < cnt ? (unsigned char)p[i + k] : 0);
        char bf[5];
        for (int k = 4; k >= 0; k--) { bf[k] = B85[ch % 85]; ch /= 85; }
        for (int k = 0; k < cnt + 1; k++) o[j++] = bf[k];
        i += 4;
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toBase91(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 2 + 1);
    int b = 0, nb = 0, j = 0;
    for (int i = 0; i < n; i++) {
        b |= (unsigned char)p[i] << nb;
        nb += 8;
        if (nb > 13) {
            int v = b & 8191;
            if (v > 88) { b >>= 13; nb -= 13; }
            else { v = b & 16383; b >>= 14; nb -= 14; }
            o[j++] = B91[v % 91];
            o[j++] = B91[v / 91];
        }
    }
    if (nb > 0) {
        o[j++] = B91[b % 91];
        if (nb > 7 || b / 91 > 0) o[j++] = B91[b / 91];
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_caesarEncrypt(JNIEnv *e, jobject t, jstring s, jint sh) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n + 1);
    for (int i = 0; i < n; i++) {
        unsigned char c = p[i];
        if (c >= 'a' && c <= 'z') c = 'a' + ((c - 'a' + sh) % 26);
        else if (c >= 'A' && c <= 'Z') c = 'A' + ((c - 'A' + sh) % 26);
        o[i] = c;
    }
    o[n] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_testToast(JNIEnv *e, jobject t) {
    return (*e)->NewStringUTF(e, "Native Lib Loaded!");
}
