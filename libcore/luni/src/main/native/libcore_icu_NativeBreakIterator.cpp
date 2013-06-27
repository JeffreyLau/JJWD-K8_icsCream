/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "NativeBreakIterator"

#include "JNIHelp.h"
#include "JniConstants.h"
#include "JniException.h"
#include "ScopedUtfChars.h"
#include "unicode/ubrk.h"
#include "unicode/putil.h"
#include <stdlib.h>

/**
 * ICU4C 4.6 doesn't let us update the pointers inside a UBreakIterator to track our char[] as it
 * moves around the heap. This class pins the char[] for the lifetime of the
 * java.text.BreakIterator. It also holds a global reference to the java.lang.String that owns the
 * char[] so that the char[] can't be GCed.
 */
class BreakIteratorPeer {
public:
    static BreakIteratorPeer* fromAddress(jint address) {
        return reinterpret_cast<BreakIteratorPeer*>(static_cast<uintptr_t>(address));
    }

    uintptr_t toAddress() {
        return reinterpret_cast<uintptr_t>(this);
    }

    BreakIteratorPeer(UBreakIterator* it) : mIt(it), mString(NULL), mChars(NULL) {
    }

    void setText(JNIEnv* env, jstring s) {
        releaseString(env);

        mString = reinterpret_cast<jstring>(env->NewGlobalRef(s));
        mChars = env->GetStringChars(mString, NULL);
        if (mChars == NULL) {
            return;
        }

        size_t charCount = env->GetStringLength(mString);
        UErrorCode status = U_ZERO_ERROR;
        ubrk_setText(mIt, mChars, charCount, &status);
        maybeThrowIcuException(env, status);
    }

    BreakIteratorPeer* clone(JNIEnv* env) {
        UErrorCode status = U_ZERO_ERROR;
        jint bufferSize = U_BRK_SAFECLONE_BUFFERSIZE;
        UBreakIterator* it = ubrk_safeClone(mIt, NULL, &bufferSize, &status);
        if (maybeThrowIcuException(env, status)) {
            return NULL;
        }
        BreakIteratorPeer* result = new BreakIteratorPeer(it);
        if (mString != NULL) {
            result->setText(env, mString);
        }
        return result;
    }

    void close(JNIEnv* env) {
        if (mIt != NULL) {
            ubrk_close(mIt);
            mIt = NULL;
        }
        releaseString(env);
    }

    ~BreakIteratorPeer() {
        if (mIt != NULL || mString != NULL) {
            LOG_ALWAYS_FATAL("BreakIteratorPeer deleted but not closed");
        }
    }

    UBreakIterator* breakIterator() {
        return mIt;
    }

private:
    UBreakIterator* mIt;

    jstring mString;
    const jchar* mChars;

    void releaseString(JNIEnv* env) {
        if (mString != NULL) {
            env->ReleaseStringChars(mString, mChars);
            env->DeleteGlobalRef(mString);
            mString = NULL;
        }
    }

    // Disallow copy and assignment.
    BreakIteratorPeer(const BreakIteratorPeer&);
    void operator=(const BreakIteratorPeer&);
};

static UBreakIterator* breakIterator(jint address) {
    return BreakIteratorPeer::fromAddress(address)->breakIterator();
}

static jint makeIterator(JNIEnv* env, jstring locale, UBreakIteratorType type) {
    UErrorCode status = U_ZERO_ERROR;
    const ScopedUtfChars localeChars(env, locale);
    if (localeChars.c_str() == NULL) {
        return 0;
    }
    UBreakIterator* it = ubrk_open(type, localeChars.c_str(), NULL, 0, &status);
    if (maybeThrowIcuException(env, status)) {
        return 0;
    }
    return (new BreakIteratorPeer(it))->toAddress();
}

static jint NativeBreakIterator_getCharacterInstanceImpl(JNIEnv* env, jclass, jstring locale) {
    return makeIterator(env, locale, UBRK_CHARACTER);
}

static jint NativeBreakIterator_getLineInstanceImpl(JNIEnv* env, jclass, jstring locale) {
    return makeIterator(env, locale, UBRK_LINE);
}

static jint NativeBreakIterator_getSentenceInstanceImpl(JNIEnv* env, jclass, jstring locale) {
    return makeIterator(env, locale, UBRK_SENTENCE);
}

static jint NativeBreakIterator_getWordInstanceImpl(JNIEnv* env, jclass, jstring locale) {
    return makeIterator(env, locale, UBRK_WORD);
}

static void NativeBreakIterator_closeBreakIteratorImpl(JNIEnv* env, jclass, jint address) {
    BreakIteratorPeer* peer = BreakIteratorPeer::fromAddress(address);
    peer->close(env);
    delete peer;
}

static jint NativeBreakIterator_cloneImpl(JNIEnv* env, jclass, jint address) {
    return BreakIteratorPeer::fromAddress(address)->clone(env)->toAddress();
}

static void NativeBreakIterator_setTextImpl(JNIEnv* env, jclass, jint address, jstring javaText) {
    BreakIteratorPeer* peer = BreakIteratorPeer::fromAddress(address);
    peer->setText(env, javaText);
}

static jboolean NativeBreakIterator_isBoundaryImpl(JNIEnv*, jclass, jint address, jint offset) {
    return ubrk_isBoundary(breakIterator(address), offset);
}

static jint NativeBreakIterator_nextImpl(JNIEnv*, jclass, jint address, jint n) {
    UBreakIterator* bi = breakIterator(address);
    if (n < 0) {
        while (n++ < -1) {
            ubrk_previous(bi);
        }
        return ubrk_previous(bi);
    } else if (n == 0) {
        return ubrk_current(bi);
    } else {
        while (n-- > 1) {
            ubrk_next(bi);
        }
        return ubrk_next(bi);
    }
    return -1;
}

static jint NativeBreakIterator_precedingImpl(JNIEnv*, jclass, jint address, jint offset) {
    return ubrk_preceding(breakIterator(address), offset);
}

static jint NativeBreakIterator_firstImpl(JNIEnv*, jclass, jint address) {
    return ubrk_first(breakIterator(address));
}

static jint NativeBreakIterator_followingImpl(JNIEnv*, jclass, jint address, jint offset) {
    return ubrk_following(breakIterator(address), offset);
}

static jint NativeBreakIterator_currentImpl(JNIEnv*, jclass, jint address) {
    return ubrk_current(breakIterator(address));
}

static jint NativeBreakIterator_previousImpl(JNIEnv*, jclass, jint address) {
    return ubrk_previous(breakIterator(address));
}

static jint NativeBreakIterator_lastImpl(JNIEnv*, jclass, jint address) {
    return ubrk_last(breakIterator(address));
}

static JNINativeMethod gMethods[] = {
    NATIVE_METHOD(NativeBreakIterator, cloneImpl, "(I)I"),
    NATIVE_METHOD(NativeBreakIterator, closeBreakIteratorImpl, "(I)V"),
    NATIVE_METHOD(NativeBreakIterator, currentImpl, "(I)I"),
    NATIVE_METHOD(NativeBreakIterator, firstImpl, "(I)I"),
    NATIVE_METHOD(NativeBreakIterator, followingImpl, "(II)I"),
    NATIVE_METHOD(NativeBreakIterator, getCharacterInstanceImpl, "(Ljava/lang/String;)I"),
    NATIVE_METHOD(NativeBreakIterator, getLineInstanceImpl, "(Ljava/lang/String;)I"),
    NATIVE_METHOD(NativeBreakIterator, getSentenceInstanceImpl, "(Ljava/lang/String;)I"),
    NATIVE_METHOD(NativeBreakIterator, getWordInstanceImpl, "(Ljava/lang/String;)I"),
    NATIVE_METHOD(NativeBreakIterator, isBoundaryImpl, "(II)Z"),
    NATIVE_METHOD(NativeBreakIterator, lastImpl, "(I)I"),
    NATIVE_METHOD(NativeBreakIterator, nextImpl, "(II)I"),
    NATIVE_METHOD(NativeBreakIterator, precedingImpl, "(II)I"),
    NATIVE_METHOD(NativeBreakIterator, previousImpl, "(I)I"),
    NATIVE_METHOD(NativeBreakIterator, setTextImpl, "(ILjava/lang/String;)V"),
};
int register_libcore_icu_NativeBreakIterator(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "libcore/icu/NativeBreakIterator", gMethods, NELEM(gMethods));
}
