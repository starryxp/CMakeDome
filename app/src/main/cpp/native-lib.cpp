#include <jni.h>
#include <string>
#include <stdio.h>
#include <string.h>


static jclass contextClass;
static jclass signatureClass;
static jclass packageNameClass;
static jclass packageInfoClass;

/**
    之前生成好的签名字符串
*/
const char *RELEASE_SIGN = "308201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3136313231353136303932395a170d3436313230383136303932395a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d0030818902818100b063554bbdc5dfd939c0e2a2c412565d1ccc1aafa433dc0c11976da9464c25e828734b00893021a045e45ad2dbab3935c17a1052253a2452e6ea85f7c8eec45aa1e073beb7cd3574edc62accdadbb2573d677c7d10123926b95711e19438be7df2756d550e333db27a056c058a7e7d00ec8b9a9a9612ad8012eea3b04777cc7b0203010001300d06092a864886f70d010105050003818100463b2a18d338e21fc310fa036931ff08bd9f3e72d33f454b182d36ad72101c16a3afef052f62726e84bdf005cf7bfd8e9c3cba3a282ebd20d38346aa6be582d9a59b83776482fb9d475f364931b797aee2d8aa9f35de1e5c9b14b8a3d0532374e26181c80c2751b0aa35c7e5315093e079fa34d6e16ae8cd9a6f8c2b0a71dd74";

/*
    根据context对象,获取签名字符串
*/
const char *getSignString(JNIEnv *env, jobject contextObject) {
    jmethodID getPackageManagerId = (env)->GetMethodID(contextClass, "getPackageManager",
                                                       "()Landroid/content/pm/PackageManager;");
    jmethodID getPackageNameId = (env)->GetMethodID(contextClass, "getPackageName",
                                                    "()Ljava/lang/String;");
    jmethodID signToStringId = (env)->GetMethodID(signatureClass, "toCharsString",
                                                  "()Ljava/lang/String;");
    jmethodID getPackageInfoId = (env)->GetMethodID(packageNameClass, "getPackageInfo",
                                                    "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject packageManagerObject = (env)->CallObjectMethod(contextObject, getPackageManagerId);
    jstring packNameString = (jstring) (env)->CallObjectMethod(contextObject, getPackageNameId);
    jobject packageInfoObject = (env)->CallObjectMethod(packageManagerObject, getPackageInfoId,
                                                        packNameString, 64);
    jfieldID signaturefieldID = (env)->GetFieldID(packageInfoClass, "signatures",
                                                  "[Landroid/content/pm/Signature;");
    jobjectArray signatureArray = (jobjectArray) (env)->GetObjectField(packageInfoObject,
                                                                       signaturefieldID);
    jobject signatureObject = (env)->GetObjectArrayElement(signatureArray, 0);
    return (env)->GetStringUTFChars(
            (jstring) (env)->CallObjectMethod(signatureObject, signToStringId), 0);
}

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_xiaopeng_cmakedome_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject jobj, jobject contextObject) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
    const char *signStrng = getSignString(env, contextObject);
    if (strcmp(signStrng, RELEASE_SIGN) == 0)//签名一致  返回合法的 api key，否则返回错误
    {
        return (env)->NewStringUTF("密钥通过，jni");
    } else {
        return (env)->NewStringUTF("error");
    }

}

/**
    利用OnLoad钩子,初始化需要用到的Class类.
*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    JNIEnv *env = NULL;
    jint result = -1;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK)
        return result;

    contextClass = (jclass) env->NewGlobalRef((env)->FindClass("android/content/Context"));
    signatureClass = (jclass) env->NewGlobalRef((env)->FindClass("android/content/pm/Signature"));
    packageNameClass = (jclass) env->NewGlobalRef(
            (env)->FindClass("android/content/pm/PackageManager"));
    packageInfoClass = (jclass) env->NewGlobalRef(
            (env)->FindClass("android/content/pm/PackageInfo"));

    return JNI_VERSION_1_4;
}
