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

.source T_getstatic_5.java
.class public dxc/junit/opcodes/getstatic/jm/T_getstatic_5
.super java/lang/Object

.field public i1 I

.method public <init>()V
    .limit stack 2
    aload_0
    invokespecial java/lang/Object/<init>()V

    aload_0
    iconst_5
    putfield dxc.junit.opcodes.getstatic.jm.T_getstatic_5.i1 I

    return
.end method


.method public run()I
    .limit stack 1
    .limit locals 1

    getstatic dxc.junit.opcodes.getstatic.jm.T_getstatic_5.i1 I

    ireturn

.end method
