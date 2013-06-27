/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.dx.gen;

import com.android.dx.rop.cst.CstFieldRef;
import com.android.dx.rop.cst.CstNat;
import com.android.dx.rop.cst.CstString;

/**
 * A field.
 */
public final class FieldId<D, V> {
    final Type<D> declaringType;
    final Type<V> type;
    final String name;

    /** cached converted state */
    final CstNat nat;
    final CstFieldRef constant;

    FieldId(Type<D> declaringType, Type<V> type, String name) {
        if (declaringType == null || type == null || name == null) {
            throw new NullPointerException();
        }
        this.declaringType = declaringType;
        this.type = type;
        this.name = name;
        this.nat = new CstNat(new CstString(name), new CstString(type.name));
        this.constant = new CstFieldRef(declaringType.constant, nat);
    }

    public Type<D> getDeclaringType() {
        return declaringType;
    }

    public Type<V> getType() {
        return type;
    }

    public String getName() {
        return name;
    }

    @Override public boolean equals(Object o) {
        return o instanceof FieldId
                && ((FieldId<?, ?>) o).declaringType.equals(declaringType)
                && ((FieldId<?, ?>) o).name.equals(name);
    }

    @Override public int hashCode() {
        return declaringType.hashCode() + 37 * name.hashCode();
    }

    @Override public String toString() {
        return declaringType + "." + name;
    }
}
