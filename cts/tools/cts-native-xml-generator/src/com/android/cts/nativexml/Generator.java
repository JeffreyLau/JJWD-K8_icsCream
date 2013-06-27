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
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Generator of TestPackage XML files for native gTests.
 *
 * It scours all the C++ source files in a given source directory looking
 * for test declarations and outputs a XML test listing.
 */
class Generator {

    /** Test package name like "android.nativemedia" to group the tests. */
    private final String mAppPackageName;

    /** Name of the native executable. */
    private final String mName;

    /** Directory to recursively scan for gTest test declarations. */
    private final File mSourceDir;

    /** Path to output file or null to just dump to standard out. */
    private final String mOutputPath;

    Generator(String appPackageName, String name, File sourceDir, String outputPath) {
        mAppPackageName = appPackageName;
        mName = name;
        mSourceDir = sourceDir;
        mOutputPath = outputPath;
    }

    public void writePackageXml() throws IOException {
        OutputStream output = System.out;
        if (mOutputPath != null) {
            File outputFile = new File(mOutputPath);
            File outputDir = outputFile.getParentFile();
            if (!outputDir.exists()) {
                outputDir.mkdirs();
                if (!outputDir.exists()) {
                    System.err.println("Couldn't make output directory: " + outputDir);
                    System.exit(1);
                }
            }
            output = new FileOutputStream(outputFile);
        }

        PrintWriter writer = null;
        try {
            writer = new PrintWriter(output);
            writer.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
            writeTestPackage(writer);
        } finally {
            if (writer != null) {
                writer.close();
            }
        }
    }

    private void writeTestPackage(PrintWriter writer) throws FileNotFoundException {
        writer.append("<TestPackage appPackageName=\"")
                .append(mAppPackageName)
                .append("\" name=\"")
                .append(mName)
                .println("\" testType=\"native\" version=\"1.0\">");
        writeTestSuite(writer);
        writer.println("</TestPackage>");
    }

    private void writeTestSuite(PrintWriter writer) throws FileNotFoundException {
        /*
         * Given "android.foo.bar.baz"...
         *
         * <TestSuite name="android">
         *   <TestSuite name="foo">
         *     <TestSuite name="bar">
         *       <TestSuite name="baz">
         */
        Scanner scanner = null;
        try {
            scanner = new Scanner(mAppPackageName);
            scanner.useDelimiter("\\.");

            int numLevels = 0;
            for (; scanner.hasNext(); numLevels++) {
                String packagePart = scanner.next();
                writer.append("<TestSuite name=\"").append(packagePart).println("\">");
            }

            writeTestCases(writer, mSourceDir);

            for (; numLevels > 0; numLevels--) {
                writer.println("</TestSuite>");
            }
        } finally {
            if (scanner != null) {
                scanner.close();
            }
        }
    }

    private void writeTestCases(PrintWriter writer, File dir) throws FileNotFoundException {
        // Find both C++ files to find tests and directories to look for more tests!
        File[] files = dir.listFiles(new FilenameFilter() {
            @Override
            public boolean accept(File dir, String filename) {
                return filename.endsWith(".cpp") || new File(dir, filename).isDirectory();
            }
        });

        for (int i = 0; i < files.length; i++) {
            File file = files[i];
            if (file.isDirectory()) {
                writeTestCases(writer, file);
            } else {
                // Take the test name from the name of the file. It's probably
                // more accurate to take the name from inside the file...
                String fileName = file.getName();
                int extension = fileName.lastIndexOf('.');
                if (extension != -1) {
                    fileName = fileName.substring(0, extension);
                }

                writer.append("<TestCase name=\"").append(fileName).println("\">");
                writeTests(writer, file);
                writer.println("</TestCase>");
            }
        }
    }

    // We want to find lines like TEST_F(SLObjectCreationTest, testAudioPlayerFromFdCreation) { ...
    // and extract the "testAudioPlayerFromFdCreation" as group #1
    private static final Pattern TEST_REGEX = Pattern.compile("\\s*TEST_F\\(\\w+,\\s*(\\w+)\\).*");

    private void writeTests(PrintWriter writer, File file) throws FileNotFoundException {
        Scanner scanner = null;
        try {
            scanner = new Scanner(file);
            while (scanner.hasNextLine()) {
                String line = scanner.nextLine();
                Matcher matcher = TEST_REGEX.matcher(line);
                if (matcher.matches()) {
                    String name = matcher.group(1);
                    writer.append("<Test name=\"").append(name).println("\" />");
                }
            }
        } finally {
            if (scanner != null) {
                scanner.close();
            }
        }
    }
}
