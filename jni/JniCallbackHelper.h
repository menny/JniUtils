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

#ifndef JniCallbackHelperH
#define JniCallbackHelperH

#include <vector>

#include <jni.h>

#ifndef JniMethodCall
#define JniMethodCall(HELPER, ...) (HELPER.getJavaEnv()->CallVoidMethod(HELPER.getJavaObject(), HELPER.getMethodID(), __VA_ARGS__))
#endif

namespace net {
namespace evendanan {
namespace jniutils {
	/*
	 * A simple helper class (and compiler pre-processor) for executing
	 */
	class JniCallbackHelper {
	private:
		jint mGetEnvResult;
		JNIEnv* mEnv;
		jobject mJObject;
		jmethodID mMethodId;
		std::vector<jobject> mNewJavaObjects;

		void init(const char* functionName, const char* signature, jobject& jobj);

	public:
		JniCallbackHelper(JavaVM* vm, jobject& jobj, const char* functionName,
				const char* signature);

		JniCallbackHelper(JNIEnv* env, jobject& jobj, const char* functionName,
						const char* signature);

		JNIEnv* getJavaEnv() {
			return mEnv;
		}
		jmethodID& getMethodID() {
			return mMethodId;
		}
		jobject& getJavaObject() {
			return mJObject;
		}

		jstring getJavaString(const char * string);

		virtual ~JniCallbackHelper();
	};
}
}
}
#endif
