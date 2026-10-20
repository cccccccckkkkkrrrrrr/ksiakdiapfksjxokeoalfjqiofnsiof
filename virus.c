#include <jni.h>
#include <android/log.h>

JNIEXPORT void JNICALL
Java_t_e_MainActivity_showToast(JNIEnv *env, jobject thiz) {
    jclass toastClass = (*env)->FindClass(env, "android/widget/Toast");
    jmethodID makeText = (*env)->GetStaticMethodID(env, toastClass, "makeText", "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    jmethodID show = (*env)->GetMethodID(env, toastClass, "show", "()V");
    jclass activityClass = (*env)->GetObjectClass(env, thiz);
    jmethodID getAppContext = (*env)->GetMethodID(env, activityClass, "getApplicationContext", "()Landroid/content/Context;");
    jobject context = (*env)->CallObjectMethod(env, thiz, getAppContext);
    jstring message = (*env)->NewStringUTF(env, "Test!!!");
    jobject toast = (*env)->CallStaticObjectMethod(env, toastClass, makeText, context, message, 0);
    (*env)->CallVoidMethod(env, toast, show);
}
