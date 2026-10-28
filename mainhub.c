#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static const char *B16 = "0123456789ABCDEF";
static const char *B32 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static const char *B36 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char *B58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const char *B62 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static const char *B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char *B85 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";
static const char *B91 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&()*+,./:;<=>?@[]^_`{|}~\"";

static int hexToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

// --- BASE ENCODERS ---

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

// --- BASE DECODERS ---

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase16(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n / 2 + 1);
    int j = 0;
    for (int i = 0; i < n; i += 2) {
        int hi = hexToInt(p[i]);
        int lo = hexToInt(p[i+1]);
        if (hi == -1 || lo == -1) break;
        o[j++] = (hi << 4) | lo;
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase2(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n / 8 + 1);
    int j = 0, val = 0, bits = 0;
    for (int i = 0; i < n; i++) {
        if (p[i] == '1') val = (val << 1) | 1;
        else if (p[i] == '0') val = val << 1;
        else continue;
        bits++;
        if (bits == 8) { o[j++] = (char)val; val = 0; bits = 0; }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase8(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n / 3 + 1);
    int j = 0, val = 0, bits = 0;
    for (int i = 0; i < n; i++) {
        if (p[i] >= '0' && p[i] <= '7') {
            val = (val << 3) | (p[i] - '0');
            bits += 3;
            if (bits >= 8) { o[j++] = (char)(val >> (bits - 8)); bits -= 8; val &= (1 << bits) - 1; }
        }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase10(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    unsigned long long v = 0;
    for (int i = 0; p[i] >= '0' && p[i] <= '9'; i++) v = v * 10 + (p[i] - '0');
    char o[64];
    int j = 0;
    if (v == 0) o[j++] = 0;
    while (v > 0) { o[j++] = (char)(v & 0xFF); v >>= 8; }
    o[j] = 0;
    for (int i = 0; i < j / 2; i++) { char tmp = o[i]; o[i] = o[j - 1 - i]; o[j - 1 - i] = tmp; }
    jstring r = (*e)->NewStringUTF(e, o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase32(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 5 / 8 + 1);
    int j = 0, val = 0, bits = 0;
    for (int i = 0; i < n; i++) {
        const char *pos = p[i] ? strchr(B32, toupper(p[i])) : NULL;
        if (!pos) continue;
        val = (val << 5) | (pos - B32);
        bits += 5;
        if (bits >= 8) { o[j++] = (char)(val >> (bits - 8)); bits -= 8; val &= (1 << bits) - 1; }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

static jstring strToBytes(JNIEnv *e, unsigned long long v) {
    char o[64];
    int j = 0;
    if (v == 0) o[j++] = 0;
    while (v > 0) { o[j++] = (char)(v & 0xFF); v >>= 8; }
    o[j] = 0;
    for (int i = 0; i < j / 2; i++) { char tmp = o[i]; o[i] = o[j - 1 - i]; o[j - 1 - i] = tmp; }
    return (*e)->NewStringUTF(e, o);
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase36(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    unsigned long long v = 0;
    for (int i = 0; p[i]; i++) {
        const char *pos = strchr(B36, toupper(p[i]));
        if (pos) v = v * 36 + (pos - B36);
    }
    jstring r = strToBytes(e, v);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase58(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    unsigned long long v = 0;
    for (int i = 0; p[i]; i++) {
        const char *pos = strchr(B58, p[i]);
        if (pos) v = v * 58 + (pos - B58);
    }
    jstring r = strToBytes(e, v);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase62(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    unsigned long long v = 0;
    for (int i = 0; p[i]; i++) {
        const char *pos = strchr(B62, p[i]);
        if (pos) v = v * 62 + (pos - B62);
    }
    jstring r = strToBytes(e, v);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase64(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 3 / 4 + 1);
    int j = 0, val = 0, bits = 0;
    for (int i = 0; i < n; i++) {
        if (p[i] == '=') break;
        const char *pos = strchr(B64, p[i]);
        if (!pos) continue;
        val = (val << 6) | (pos - B64);
        bits += 6;
        if (bits >= 8) { o[j++] = (char)(val >> (bits - 8)); bits -= 8; val &= (1 << bits) - 1; }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase85(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 4 / 5 + 1);
    int i = 0, j = 0;
    while (i < n) {
        unsigned long long ch = 0;
        int cnt = 0;
        for (int k = 0; k < 5 && i < n; k++, i++) {
            const char *pos = strchr(B85, p[i]);
            if (!pos) break;
            ch = ch * 85 + (pos - B85);
            cnt++;
        }
        for (int k = 3; k >= 4 - (cnt - 1); k--) {
            o[j++] = (char)((ch >> (k * 8)) & 0xFF);
        }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromBase91(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n + 1);
    int b = 0, nb = 0, j = 0, v = -1;
    for (int i = 0; i < n; i++) {
        const char *pos = strchr(B91, p[i]);
        if (!pos) continue;
        int c = pos - B91;
        if (v < 0) { v = c; }
        else {
            v += c * 91;
            b |= v << nb;
            nb += (v & 8191) > 88 ? 13 : 14;
            do {
                o[j++] = (char)(b & 0xFF);
                b >>= 8;
                nb -= 8;
            } while (nb > 7);
            v = -1;
        }
    }
    if (v != -1) {
        b |= v << nb;
        o[j++] = (char)(b & 0xFF);
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

// --- CAESAR CIPHER ---

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_caesarEncrypt(JNIEnv *e, jobject t, jstring s, jint sh) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n + 1);
    for (int i = 0; i < n; i++) {
        unsigned char c = p[i];
        if (c >= 'a' && c <= 'z') c = 'a' + ((c - 'a' + sh) % 26 + 26) % 26;
        else if (c >= 'A' && c <= 'Z') c = 'A' + ((c - 'A' + sh) % 26 + 26) % 26;
        else if (c >= '0' && c <= '9') c = '0' + ((c - '0' + sh) % 10 + 10) % 10;
        o[i] = c;
    }
    o[n] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_caesarDecrypt(JNIEnv *e, jobject t, jstring s, jint sh) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n + 1);
    for (int i = 0; i < n; i++) {
        unsigned char c = p[i];
        if (c >= 'a' && c <= 'z') c = 'a' + ((c - 'a' - sh) % 26 + 26) % 26;
        else if (c >= 'A' && c <= 'Z') c = 'A' + ((c - 'A' - sh) % 26 + 26) % 26;
        else if (c >= '0' && c <= '9') c = '0' + ((c - '0' - sh) % 10 + 10) % 10;
        o[i] = c;
    }
    o[n] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

// --- MORSE CODE ---

static const char *MORSE_LATIN[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--.."
};
static const char *MORSE_LATIN_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static const char *MORSE_CYRILLIC_CHARS = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
static const char *MORSE_CYRILLIC[] = {
    ".-", "-...", ".--", "--.", "-..", ".", ".", "...-", "--..", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", ".-.", "...", "-", "..-",
    "..-.", "....", "-.-.", "---.", "----", "--.-", "--.--", "-.--", "-..-", "..-..", "..--", ".-.-"
};

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_toMorse(JNIEnv *e, jobject t, jstring s, jboolean cyrillic) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n * 6 + 1);
    int j = 0;
    for (int i = 0; i < n; i++) {
        char c = toupper(p[i]);
        if (c == ' ') { o[j++] = '/'; o[j++] = ' '; continue; }
        int found = 0;
        
        if (cyrillic) {
            const char *pos = strchr(MORSE_CYRILLIC_CHARS, c);
            if (pos) {
                int idx = pos - MORSE_CYRILLIC_CHARS;
                const char *m = MORSE_CYRILLIC[idx];
                while (*m) o[j++] = *m++;
                o[j++] = ' ';
                found = 1;
            }
        }
        
        if (!found) {
            const char *pos = strchr(MORSE_LATIN_CHARS, c);
            if (pos) {
                int idx = pos - MORSE_LATIN_CHARS;
                const char *m = MORSE_LATIN[idx];
                while (*m) o[j++] = *m++;
                o[j++] = ' ';
                found = 1;
            }
        }
        
        if (!found) { o[j++] = '?'; o[j++] = ' '; }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_fromMorse(JNIEnv *e, jobject t, jstring s, jboolean cyrillic) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n + 1);
    int j = 0;
    char buf[8];
    int idx = 0;
    
    for (int i = 0; i <= n; i++) {
        char c = p[i];
        if (c == ' ' || c == '/' || c == '\0') {
            if (idx > 0) {
                buf[idx] = '\0';
                int found = 0;
                if (cyrillic) {
                    for (int k = 0; k < 33; k++) {
                        if (strcmp(MORSE_CYRILLIC[k], buf) == 0) {
                            o[j++] = MORSE_CYRILLIC_CHARS[k];
                            found = 1;
                            break;
                        }
                    }
                }
                if (!found) {
                    for (int k = 0; k < 26; k++) {
                        if (strcmp(MORSE_LATIN[k], buf) == 0) {
                            o[j++] = MORSE_LATIN_CHARS[k];
                            found = 1;
                            break;
                        }
                    }
                }
                if (!found) o[j++] = '?';
                idx = 0;
            }
            if (c == '/') o[j++] = ' ';
        } else {
            buf[idx++] = c;
        }
    }
    o[j] = 0;
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}

// --- COOL THINGS ---

JNIEXPORT jstring JNICALL
Java_pbcats_hub_official_android_app_jqNbTiJfRiGzfRIFgT_NativeLib_reverseText(JNIEnv *e, jobject t, jstring s) {
    const char *p = (*e)->GetStringUTFChars(e, s, 0);
    int n = strlen(p);
    char *o = (char*)malloc(n + 1);
    
    int byteIdx = 0;
    int charCount = 0;
    for (int i = 0; i < n; i++) {
        if ((p[i] & 0xC0) != 0x80) charCount++;
    }
    int *indices = (int*)malloc((charCount + 1) * sizeof(int));
    int c = 0;
    for (int i = 0; i < n; i++) {
        if ((p[i] & 0xC0) != 0x80) indices[c++] = i;
    }
    indices[charCount] = n;
    
    int pos = 0;
    for (int i = charCount - 1; i >= 0; i--) {
        int start = indices[i];
        int end = indices[i+1];
        memcpy(o + pos, p + start, end - start);
        pos += end - start;
    }
    o[pos] = 0;
    
    free(indices);
    jstring r = (*e)->NewStringUTF(e, o);
    free(o);
    (*e)->ReleaseStringUTFChars(e, s, p);
    return r;
}