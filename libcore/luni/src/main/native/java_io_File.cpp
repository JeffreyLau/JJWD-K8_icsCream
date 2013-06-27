/*
 *  Licensed to the Apache Software Foundation (ASF) under one or more
 *  contributor license agreements.  See the NOTICE file distributed with
 *  this work for additional information regarding copyright ownership.
 *  The ASF licenses this file to You under the Apache License, Version 2.0
 *  (the "License"); you may not use this file except in compliance with
 *  the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#define LOG_TAG "File"

#include "JNIHelp.h"
#include "JniConstants.h"
#include "JniException.h"
#include "ScopedPrimitiveArray.h"
#include "ScopedUtfChars.h"
#include "readlink.h"
#include "toStringArray.h"

#include <string>
#include <vector>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

static jstring File_readlink(JNIEnv* env, jclass, jstring javaPath) {
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return NULL;
    }

    std::string result;
    if (!readlink(path.c_str(), result)) {
        jniThrowIOException(env, errno);
        return NULL;
    }
    return env->NewStringUTF(result.c_str());
}

static jstring File_realpath(JNIEnv* env, jclass, jstring javaPath) {
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return NULL;
    }

    extern bool realpath(const char* path, std::string& resolved);
    std::string result;
    if (!realpath(path.c_str(), result)) {
        jniThrowIOException(env, errno);
        return NULL;
    }
    return env->NewStringUTF(result.c_str());
}

static jboolean File_setLastModifiedImpl(JNIEnv* env, jclass, jstring javaPath, jlong ms) {
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return JNI_FALSE;
    }

    // We want to preserve the access time.
    struct stat sb;
    if (stat(path.c_str(), &sb) == -1) {
        return JNI_FALSE;
    }

    // TODO: we could get microsecond resolution with utimes(3), "legacy" though it is.
    utimbuf times;
    times.actime = sb.st_atime;
    times.modtime = static_cast<time_t>(ms / 1000);
    return (utime(path.c_str(), &times) == 0);
}

// Iterates over the filenames in the given directory.
class ScopedReaddir {
public:
    ScopedReaddir(const char* path) {
        mDirStream = opendir(path);
        mIsBad = (mDirStream == NULL);
    }

    ~ScopedReaddir() {
        if (mDirStream != NULL) {
            closedir(mDirStream);
        }
    }

    // Returns the next filename, or NULL.
    const char* next() {
        dirent* result = NULL;
        int rc = readdir_r(mDirStream, &mEntry, &result);
        if (rc != 0) {
            mIsBad = true;
            return NULL;
        }
        return (result != NULL) ? result->d_name : NULL;
    }

    // Has an error occurred on this stream?
    bool isBad() const {
        return mIsBad;
    }

private:
    DIR* mDirStream;
    dirent mEntry;
    bool mIsBad;

    // Disallow copy and assignment.
    ScopedReaddir(const ScopedReaddir&);
    void operator=(const ScopedReaddir&);
};

typedef std::vector<std::string> DirEntries;

// Reads the directory referred to by 'pathBytes', adding each directory entry
// to 'entries'.
static bool readDirectory(JNIEnv* env, jstring javaPath, DirEntries& entries) {
    ScopedUtfChars path(env, javaPath);
    if (path.c_str() == NULL) {
        return false;
    }

    ScopedReaddir dir(path.c_str());
    if (dir.isBad()) {
        return false;
    }
    const char* filename;
    while ((filename = dir.next()) != NULL) {
        if (strcmp(filename, ".") != 0 && strcmp(filename, "..") != 0) {
            // TODO: this hides allocation failures from us. Push directory iteration up into Java?
            entries.push_back(filename);
        }
    }
    return true;
}

static jobjectArray File_listImpl(JNIEnv* env, jclass, jstring javaPath) {
    // Read the directory entries into an intermediate form.
    DirEntries entries;
    if (!readDirectory(env, javaPath, entries)) {
        return NULL;
    }
    // Translate the intermediate form into a Java String[].
    return toStringArray(env, entries);
}

static JNINativeMethod gMethods[] = {
    NATIVE_METHOD(File, listImpl, "(Ljava/lang/String;)[Ljava/lang/String;"),
    NATIVE_METHOD(File, readlink, "(Ljava/lang/String;)Ljava/lang/String;"),
    NATIVE_METHOD(File, realpath, "(Ljava/lang/String;)Ljava/lang/String;"),
    NATIVE_METHOD(File, setLastModifiedImpl, "(Ljava/lang/String;J)Z"),
};
int register_java_io_File(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "java/io/File", gMethods, NELEM(gMethods));
}
