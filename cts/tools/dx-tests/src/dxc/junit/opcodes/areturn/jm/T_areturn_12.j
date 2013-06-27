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

.source T_areturn_12.java
.class public dxc/junit/opcodes/areturn/jm/T_areturn_12
.super java/lang/Object

.method public <init>()V
    aload_0
    invokespecial java/lang/Object/<init>()V
    return
.end method



.method private test()Ldxc/junit/opcodes/areturn/jm/TSuper;
    .limit stack 2
    .limit locals 1

    new dxc/junit/opcodes/areturn/jm/TChild
    dup
    invokespecial dxc/junit/opcodes/areturn/jm/TChild/<init>()V

    areturn
.end method



.method public run()Z
    .limit stack 1
    .limit locals 2

    aload_0
    invokespecial dxc/junit/opcodes/areturn/jm/T_areturn_12/test()Ldxc/junit/opcodes/areturn/jm/TSuper;

    instanceof dxc/junit/opcodes/areturn/jm/TChild

    ireturn
.end method
