/*
 * Copyright 2012, Menny Even Danan
 * https://github.com/menny/JniUtils
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *
 *
 */

#include <jni.h>
#include <android/log.h>
#include "JniCallbackHelper.h"
#include <unistd.h>

/* Missing (from NDK) log macros (cutils/log.h) */
#ifndef JNI_HELPER_LOGTAG
#define JNI_HELPER_LOGTAG "JniCallbackHelper"
#endif

#ifndef JNI_HELPER_LOGD
#define JNI_HELPER_LOGD(...) (__android_log_print(ANDROID_LOG_DEBUG, JNI_HELPER_LOGTAG, __VA_ARGS__))
#endif

#ifndef JNI_HELPER_LOGI
#define JNI_HELPER_LOGI(...) (__android_log_print(ANDROID_LOG_INFO, JNI_HELPER_LOGTAG, __VA_ARGS__))
#endif

#ifndef JNI_HELPER_LOGE
#define JNI_HELPER_LOGE(...) (__android_log_print(ANDROID_LOG_ERROR, JNI_HELPER_LOGTAG, __VA_ARGS__))
#endif

using namespace net::evendanan::jniutils;

JniCallbackHelper::JniCallbackHelper(JavaVM* vm, jobject& jobj,
		const char* functionName, const char* signature) {
	mGetEnvResult = vm->GetEnv((void**) &mEnv, JNI_VERSION_1_2);
	if (mGetEnvResult == JNI_EDETACHED) {
		//thread attaching is required.
		vm->AttachCurrentThread(&mEnv, NULL);
	}
	if (mEnv == NULL) {
		JNI_HELPER_LOGE("JniCallbackHelper ctor could not retrieve mEnv!");
	}

	init(functionName, signature, jobj);
}

JniCallbackHelper::JniCallbackHelper(JNIEnv* env, jobject& jobj,
		const char* functionName, const char* signature) {
	mGetEnvResult = 0;
	mEnv = env;

	init(functionName, signature, jobj);
}

JniCallbackHelper::~JniCallbackHelper() {

	if (mMethodId != 0) {
		if (mEnv->ExceptionCheck()) {
			JNI_HELPER_LOGE(
					"There was an exception thrown from the Java method!!");
			mEnv->ExceptionClear();
		}
	}
	for (std::vector<jobject>::iterator it = mNewJavaObjects.begin();
			it != mNewJavaObjects.end(); ++it) {
		mEnv->DeleteLocalRef(*it);
	}

	mEnv->DeleteGlobalRef(mJObject);

	if (JNI_EDETACHED == mGetEnvResult) {
		JavaVM * vm;
		mEnv->GetJavaVM(&vm);
		vm->DetachCurrentThread();
	}
}

void JniCallbackHelper::init(const char* functionName, const char* signature, jobject& jobj) {
	mJObject = mEnv->NewGlobalRef(jobj);
	if(!mJObject) {
		JNI_HELPER_LOGE("***** could not get global ref for jobj!!!");
	}
	jclass jcls = mEnv->GetObjectClass(jobj);
	mMethodId = mEnv->GetMethodID(jcls, functionName, signature);
	if (mMethodId == 0) {
		JNI_HELPER_LOGE("Failed to get Java %s (%s)", functionName, signature);
	}
}

jstring JniCallbackHelper::getJavaString(const char * nativeString) {
	JNI_HELPER_LOGD("Creating jString from '%s'", nativeString);
	jstring javaString = mEnv->NewStringUTF(nativeString);
	if (!javaString) {
		JNI_HELPER_LOGE("NewStringUTF failed");
	} else {
		mNewJavaObjects.push_back(javaString);
		JNI_HELPER_LOGD("Created a jstring.");
	}
	return javaString;
}
