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
package com.android.cts.nativexml;

import java.io.File;
import java.util.Arrays;

/**
 * Class that searches a source directory for native gTests and outputs a
 * test package xml.
 */
public class CtsNativeXmlGenerator {

    private static void usage(String[] args) {
        System.err.println("Arguments: " + Arrays.asList(args));
        System.err.println("Usage: cts-native-xml-generator -p PACKAGE_NAME -n EXECUTABLE_NAME "
                + "-s SOURCE_DIR [-o OUTPUT_FILE]");
        System.exit(1);
    }

    public static void main(String[] args) throws Exception {
        String appPackageName = null;
        String name = null;
        File sourceDir = null;
        String outputPath = null;

        for (int i = 0; i < args.length; i++) {
            if ("-p".equals(args[i])) {
                if (i + 1 < args.length) {
                    appPackageName = args[++i];
                } else {
                    System.err.println("Missing value for test package");
                    usage(args);
                }
            } else if ("-n".equals(args[i])) {
                if (i + 1 < args.length) {
                    name = args[++i];
                } else {
                    System.err.println("Missing value for executable name");
                    usage(args);
                }
            } else if ("-o".equals(args[i])) {
                if (i + 1 < args.length) {
                    outputPath = args[++i];
                } else {
                    System.err.println("Missing value for output file");
                    usage(args);
                }
            } else if ("-s".equals(args[i])) {
                if (i + 1 < args.length) {
                    sourceDir = new File(args[++i]);
                } else {
                    System.err.println("Missing value for source directory");
                    usage(args);
                }
            } else {
                System.err.println("Unsupported flag: " + args[i]);
                usage(args);
            }
        }

        if (appPackageName == null) {
            System.out.println("Package name is required");
            usage(args);
        } else if (name == null) {
            System.out.println("Executable name is required");
            usage(args);
        } else if (sourceDir == null) {
            System.out.println("Source directory is required");
            usage(args);
        }

        Generator generator = new Generator(appPackageName, name, sourceDir, outputPath);
        generator.writePackageXml();
    }
}
