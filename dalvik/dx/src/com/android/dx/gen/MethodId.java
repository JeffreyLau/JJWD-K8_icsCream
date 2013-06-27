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

import com.android.dx.rop.cst.CstMethodRef;
import com.android.dx.rop.cst.CstNat;
import com.android.dx.rop.cst.CstString;
import com.android.dx.rop.type.Prototype;
import java.util.List;

/**
 * A method or constructor.
 */
public final class MethodId<D, R> {
    final Type<D> declaringType;
    final Type<R> returnType;
    final String name;
    final TypeList parameters;

    /** cached converted state */
    final CstNat nat;
    final CstMethodRef constant;

    MethodId(Type<D> declaringType, Type<R> returnType, String name, TypeList parameters) {
        if (declaringType == null || returnType == null || name == null || parameters == null) {
            throw new NullPointerException();
        }
        this.declaringType = declaringType;
        this.returnType = returnType;
        this.name = name;
        this.parameters = parameters;
        this.nat = new CstNat(new CstString(name), new CstString(descriptor(false)));
        this.constant = new CstMethodRef(declaringType.constant, nat);
    }

    public Type<D> getDeclaringType() {
        return declaringType;
    }

    public Type<R> getReturnType() {
        return returnType;
    }

    public String getName() {
        return name;
    }

    public List<Type<?>> getParameters() {
        return parameters.asList();
    }

    /**
     * Returns a descriptor like "(Ljava/lang/Class;[I)Ljava/lang/Object;".
     */
    String descriptor(boolean includeThis) {
        StringBuilder result = new StringBuilder();
        result.append("(");
        if (includeThis) {
            result.append(declaringType.name);
        }
        for (Type t : parameters.types) {
            result.append(t.name);
        }
        result.append(")");
        result.append(returnType.name);
        return result.toString();
    }

    Prototype prototype(boolean includeThis) {
        return Prototype.intern(descriptor(includeThis));
    }

    @Override public boolean equals(Object o) {
        return o instanceof MethodId
                && ((MethodId<?, ?>) o).declaringType.equals(declaringType)
                && ((MethodId<?, ?>) o).name.equals(name)
                && ((MethodId<?, ?>) o).parameters.equals(parameters)
                && ((MethodId<?, ?>) o).returnType.equals(returnType);
    }

    @Override public int hashCode() {
        int result = 17;
        result = 31 * result + declaringType.hashCode();
        result = 31 * result + name.hashCode();
        result = 31 * result + parameters.hashCode();
        result = 31 * result + returnType.hashCode();
        return result;
    }

    @Override public String toString() {
        return declaringType + "." + name + "(" + parameters + ")";
    }
}
