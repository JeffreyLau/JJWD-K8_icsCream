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

import com.android.dx.dex.DexFormat;
import com.android.dx.dex.DexOptions;
import com.android.dx.dex.code.DalvCode;
import com.android.dx.dex.code.PositionList;
import com.android.dx.dex.code.RopTranslator;
import com.android.dx.dex.file.ClassDefItem;
import com.android.dx.dex.file.DexFile;
import com.android.dx.dex.file.EncodedField;
import com.android.dx.dex.file.EncodedMethod;
import com.android.dx.rop.code.AccessFlags;
import static com.android.dx.rop.code.AccessFlags.*;
import com.android.dx.rop.code.LocalVariableInfo;
import com.android.dx.rop.code.RopMethod;
import com.android.dx.rop.cst.CstString;
import com.android.dx.rop.cst.CstType;
import com.android.dx.rop.type.StdTypeList;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.jar.JarEntry;
import java.util.jar.JarOutputStream;

/**
 * Define types, fields and methods.
 */
public final class DexGenerator {
    private final Map<Type<?>, TypeDeclaration> types
            = new LinkedHashMap<Type<?>, TypeDeclaration>();

    private TypeDeclaration getTypeDeclaration(Type<?> type) {
        TypeDeclaration result = types.get(type);
        if (result == null) {
            result = new TypeDeclaration(type);
            types.put(type, result);
        }
        return result;
    }

    /**
     * @param flags any flags masked by {@link com.android.dx.rop.code.AccessFlags#CLASS_FLAGS}.
     */
    public void declare(Type<?> type, String sourceFile, int flags,
            Type<?> supertype, Type<?>... interfaces) {
        TypeDeclaration declaration = getTypeDeclaration(type);
        if (declaration.declared) {
            throw new IllegalStateException("already declared: " + type);
        }
        declaration.declared = true;
        declaration.flags = flags;
        declaration.supertype = supertype;
        declaration.sourceFile = sourceFile;
        declaration.interfaces = new TypeList(interfaces);
    }

    /**
     * @param flags any flags masked by {@link com.android.dx.rop.code.AccessFlags#METHOD_FLAGS}.
     */
    public Code declare(MethodId<?, ?> method, int flags) {
        TypeDeclaration typeDeclaration = getTypeDeclaration(method.declaringType);
        if (typeDeclaration.methods.containsKey(method)) {
            throw new IllegalStateException("already declared: " + method);
        }
        MethodDeclaration methodDeclaration = new MethodDeclaration(method, flags);
        typeDeclaration.methods.put(method, methodDeclaration);
        return methodDeclaration.code;
    }

    /**
     * @param flags any flags masked by {@link AccessFlags#FIELD_FLAGS}.
     */
    public void declare(FieldId<?, ?> fieldId, int flags, Object staticValue) {
        TypeDeclaration typeDeclaration = getTypeDeclaration(fieldId.declaringType);
        if (typeDeclaration.fields.containsKey(fieldId)) {
            throw new IllegalStateException("already declared: " + fieldId);
        }
        FieldDeclaration fieldDeclaration = new FieldDeclaration(fieldId, flags, staticValue);
        typeDeclaration.fields.put(fieldId, fieldDeclaration);
    }

    /**
     * Returns a .dex formatted file.
     */
    public byte[] generate() {
        DexFile outputDex = new DexFile(new DexOptions());

        for (TypeDeclaration typeDeclaration : types.values()) {
            outputDex.add(typeDeclaration.toClassDefItem());
        }

        try {
            return outputDex.toDex(null, false);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Loads the generated types into the current dalvikvm process.
     */
    public ClassLoader load(ClassLoader parent) throws IOException {
        byte[] dex = generate();

        /*
         * This implementation currently dumps the dex to the filesystem. It
         * jars the emitted .dex for the benefit of Gingerbread and earlier
         * devices, which can't load .dex files directly.
         *
         * TODO: load the dex from memory where supported.
         */
        File result = File.createTempFile("Generated", ".jar");
        result.deleteOnExit();
        JarOutputStream jarOut = new JarOutputStream(new FileOutputStream(result));
        jarOut.putNextEntry(new JarEntry(DexFormat.DEX_IN_JAR_NAME));
        jarOut.write(dex);
        jarOut.closeEntry();
        jarOut.close();
        try {
            Class<?> pathClassLoader = Class.forName("dalvik.system.PathClassLoader");
            return (ClassLoader) pathClassLoader.getConstructor(String.class, ClassLoader.class)
                    .newInstance(result.getPath(), parent);
        } catch (ClassNotFoundException e) {
            throw new UnsupportedOperationException("load() requires a Dalvik VM", e);
        } catch (InvocationTargetException e) {
            throw new RuntimeException(e.getCause());
        } catch (InstantiationException e) {
            throw new AssertionError();
        } catch (NoSuchMethodException e) {
            throw new AssertionError();
        } catch (IllegalAccessException e) {
            throw new AssertionError();
        }
    }

    private static class TypeDeclaration {
        private final Type<?> type;

        /** declared state */
        private boolean declared;
        private int flags;
        private Type<?> supertype;
        private String sourceFile;
        private TypeList interfaces;

        private final Map<FieldId, FieldDeclaration> fields
                = new LinkedHashMap<FieldId, FieldDeclaration>();
        private final Map<MethodId, MethodDeclaration> methods
                = new LinkedHashMap<MethodId, MethodDeclaration>();

        TypeDeclaration(Type<?> type) {
            this.type = type;
        }

        ClassDefItem toClassDefItem() {
            if (!declared) {
                throw new IllegalStateException("Undeclared type " + type + " declares members: "
                        + fields.keySet() + " " + methods.keySet());
            }

            DexOptions dexOptions = new DexOptions();
            dexOptions.targetApiLevel = DexFormat.API_NO_EXTENDED_OPCODES;

            CstType thisType = type.constant;

            ClassDefItem out = new ClassDefItem(thisType, flags, supertype.constant,
                    interfaces.ropTypes, new CstString(sourceFile));

            for (MethodDeclaration method : methods.values()) {
                EncodedMethod encoded = method.toEncodedMethod(dexOptions);
                if (method.isDirect()) {
                    out.addDirectMethod(encoded);
                } else {
                    out.addVirtualMethod(encoded);
                }
            }
            for (FieldDeclaration field : fields.values()) {
                EncodedField encoded = field.toEncodedField();
                if (field.isStatic()) {
                    out.addStaticField(encoded, Constants.getConstant(field.staticValue));
                } else {
                    out.addInstanceField(encoded);
                }
            }

            return out;
        }
    }

    static class FieldDeclaration {
        final FieldId<?, ?> fieldId;
        private final int accessFlags;
        private final Object staticValue;

        FieldDeclaration(FieldId<?, ?> fieldId, int accessFlags, Object staticValue) {
            if ((accessFlags & (AccessFlags.ACC_STATIC)) == 0 && staticValue != null) {
                throw new IllegalArgumentException("instance fields may not have a value");
            }
            this.fieldId = fieldId;
            this.accessFlags = accessFlags;
            this.staticValue = staticValue;
        }

        EncodedField toEncodedField() {
            return new EncodedField(fieldId.constant, accessFlags);
        }

        public boolean isStatic() {
            return (accessFlags & (AccessFlags.ACC_STATIC)) != 0;
        }
    }

    static class MethodDeclaration {
        final MethodId<?, ?> method;
        private final int flags;
        private final Code code;

        public MethodDeclaration(MethodId<?, ?> method, int flags) {
            this.method = method;
            this.flags = flags;
            this.code = new Code(this);
        }

        boolean isStatic() {
            return (flags & ACC_STATIC) != 0;
        }

        boolean isDirect() {
            return (flags & (ACC_STATIC | ACC_PRIVATE | ACC_CONSTRUCTOR)) != 0;
        }

        EncodedMethod toEncodedMethod(DexOptions dexOptions) {
            RopMethod ropMethod = new RopMethod(code.toBasicBlocks(), 0);
            LocalVariableInfo locals = null;
            DalvCode dalvCode = RopTranslator.translate(
                    ropMethod, PositionList.NONE, locals, code.paramSize(), dexOptions);
            return new EncodedMethod(method.constant, flags, dalvCode, StdTypeList.EMPTY);
        }
    }
}
