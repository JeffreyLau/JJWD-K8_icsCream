; Copyright (C) 2008 The Android Open Source Project
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;      http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.

.source T_fload_1_6.java
.class public dxc/junit/opcodes/fload_1/jm/T_fload_1_6
.super java/lang/Object

.method public <init>()V
    aload_0
    invokespecial java/lang/Object/<init>()V
    return
.end method

.method public static run()Z
    .limit stack 2
    .limit locals 2

    fconst_2
    fstore_1

    fload_1
    fload 1
    fcmpl 
    ifne Label0
    iconst_1
    ireturn

    Label0:
    iconst_0
    ireturn

.end method
