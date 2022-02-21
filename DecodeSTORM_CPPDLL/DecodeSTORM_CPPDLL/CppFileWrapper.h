#pragma once
#include<jni.h>
#include <vector>
#include <string>

using namespace std;

std::string jstring2str(JNIEnv* env, jstring jstr);
