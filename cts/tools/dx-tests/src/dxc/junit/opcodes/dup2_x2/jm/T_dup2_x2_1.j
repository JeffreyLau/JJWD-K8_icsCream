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

.source T_dup2_x2_1.java
.class public dxc/junit/opcodes/dup2_x2/jm/T_dup2_x2_1
.super java/lang/Object

.method public <init>()V
    aload_0
    invokespecial java/lang/Object/<init>()V
    return
.end method

.method public run()Z
    .limit stack 8
    .limit locals 2
    
    iconst_2
    iconst_3
    iconst_4
    iconst_5    ; 2 3 4 5
    dup2_x2        ; 4 5 2 3 4 5
    iconst_5
    if_icmpne Label5    ; 4 5 2 3 4 
    iconst_4
    if_icmpne Label4    ; 4 5 2 3 
    iconst_3
    if_icmpne Label3    ; 4 5 2 
    iconst_2
    if_icmpne Label2    ; 4 5
    iconst_5
    if_icmpne Label1    ; 4
    iconst_4
    if_icmpne Label0    ; 4

    iconst_1
    ireturn
   
Label5:
    pop 
Label4:
    pop
Label3:
    pop
Label2:
    pop
Label1:
    pop
Label0:    
    iconst_0
    ireturn
        
.end method
