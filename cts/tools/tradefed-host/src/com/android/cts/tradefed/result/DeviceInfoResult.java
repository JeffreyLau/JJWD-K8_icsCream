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
package com.android.cts.tradefed.result;

import android.tests.getinfo.DeviceInfoConstants;

import com.android.tradefed.log.LogUtil.CLog;

import org.kxml2.io.KXmlSerializer;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

/**
 * Data structure for the device info collected by CTS.
 * <p/>
 * Provides methods to serialize and deserialize from XML, as well as checks for consistency
 * when multiple devices are used to generate the report.
 */
class DeviceInfoResult extends AbstractXmlPullParser {
    static final String TAG = "DeviceInfo";
    private static final String ns = CtsXmlResultReporter.ns;
    static final String BUILD_TAG = "BuildInfo";
    private static final String PHONE_TAG = "PhoneSubInfo";
    private static final String SCREEN_TAG = "Screen";
    private static final String FEATURE_INFO_TAG = "FeatureInfo";
    private static final String FEATURE_TAG = "Feature";
    private static final String FEATURE_ATTR_DELIM = ":";
    private static final String FEATURE_DELIM = ";";
    private static final String OPENGL_TEXTURE_FORMATS_INFO_TAG =
            "OpenGLCompressedTextureFormatsInfo";
    private static final String OPENGL_TEXTURE_FORMAT_TAG = "TextureFormat";
    private static final String OPENGL_TEXTURE_FORMAT_DELIM = ";";
    private static final String SYSLIB_INFO_TAG = "SystemLibrariesInfo";
    private static final String SYSLIB_TAG = "Library";
    private static final String SYSLIB_DELIM = ";";
    private static final String PROCESS_INFO_TAG = "ProcessInfo";
    private static final String PROCESS_TAG = "Process";
    private static final String PROCESS_DELIM = ";";

    private Map<String, String> mMetrics = new HashMap<String, String>();

    /**
     * Serialize this object and all its contents to XML.
     *
     * @param serializer
     * @throws IOException
     */
    public void serialize(KXmlSerializer serializer) throws IOException {
        serializer.startTag(ns, TAG);

        if (!mMetrics.isEmpty()) {

            // Extract metrics that need extra handling, and then dump the remainder into BuildInfo
            Map<String, String> metricsCopy = new HashMap<String, String>(mMetrics);
            serializer.startTag(ns, SCREEN_TAG);
            serializer.attribute(ns, DeviceInfoConstants.RESOLUTION,
                    getMetric(metricsCopy, DeviceInfoConstants.RESOLUTION));
            serializer.attribute(ns, DeviceInfoConstants.SCREEN_DENSITY,
                    getMetric(metricsCopy, DeviceInfoConstants.SCREEN_DENSITY));
            serializer.attribute(ns, DeviceInfoConstants.SCREEN_DENSITY_BUCKET,
                    getMetric(metricsCopy, DeviceInfoConstants.SCREEN_DENSITY_BUCKET));
            serializer.attribute(ns, DeviceInfoConstants.SCREEN_SIZE,
                    getMetric(metricsCopy, DeviceInfoConstants.SCREEN_SIZE));
            serializer.endTag(ns, SCREEN_TAG);

            serializer.startTag(ns, PHONE_TAG);
            serializer.attribute(ns, DeviceInfoConstants.PHONE_NUMBER,
                    getMetric(metricsCopy, DeviceInfoConstants.PHONE_NUMBER));
            serializer.endTag(ns, PHONE_TAG);

            String featureData = getMetric(metricsCopy, DeviceInfoConstants.FEATURES);
            String processData = getMetric(metricsCopy, DeviceInfoConstants.PROCESSES);
            String sysLibData = getMetric(metricsCopy, DeviceInfoConstants.SYS_LIBRARIES);
            String textureData = getMetric(metricsCopy,
                    DeviceInfoConstants.OPEN_GL_COMPRESSED_TEXTURE_FORMATS);

            // dump the remaining metrics without translation
            serializer.startTag(ns, BUILD_TAG);
            for (Map.Entry<String, String> metricEntry : metricsCopy.entrySet()) {
                serializer.attribute(ns, metricEntry.getKey(), metricEntry.getValue());
            }
            serializer.endTag(ns, BUILD_TAG);

            serializeFeatureInfo(serializer, featureData);
            serializeProcessInfo(serializer, processData);
            serializeSystemLibrariesInfo(serializer, sysLibData);
            serializeOpenGLCompressedTextureFormatsInfo(serializer, textureData);
        } else {
            // this might be expected, if device info collection was turned off
            CLog.d("Could not find device info");
        }
        serializer.endTag(ns, TAG);
    }

    /**
     * Fetch and remove given metric from hashmap.
     *
     * @return the metric value or empty string if it was not present in map.
     */
    private String getMetric(Map<String, String> metrics, String metricName ) {
        String value = metrics.remove(metricName);
        if (value == null) {
            value = "";
        }
        return value;
    }

    /**
     * Prints XML indicating what features are supported by the device. It parses a string from the
     * featureData argument that is in the form of "feature1:true;feature2:false;featuer3;true;"
     * with a trailing semi-colon.
     *
     * <pre>
     *  <FeatureInfo>
     *     <Feature name="android.name.of.feature" available="true" />
     *     ...
     *   </FeatureInfo>
     * </pre>
     *
     * @param serializer used to create XML
     * @param featureData raw unparsed feature data
     */
    private void serializeFeatureInfo(KXmlSerializer serializer, String featureData)
            throws IOException {
        serializer.startTag(ns, FEATURE_INFO_TAG);

        if (featureData == null) {
            featureData = "";
        }

        String[] featurePairs = featureData.split(FEATURE_DELIM);
        for (String featurePair : featurePairs) {
            String[] nameTypeAvailability = featurePair.split(FEATURE_ATTR_DELIM);
            if (nameTypeAvailability.length >= 3) {
                serializer.startTag(ns, FEATURE_TAG);
                serializer.attribute(ns, "name", nameTypeAvailability[0]);
                serializer.attribute(ns, "type", nameTypeAvailability[1]);
                serializer.attribute(ns, "available", nameTypeAvailability[2]);
                serializer.endTag(ns, FEATURE_TAG);
            }
        }
        serializer.endTag(ns, FEATURE_INFO_TAG);
    }

    /**
     * Prints XML data indicating what particular processes of interest were running on the device.
     * It parses a string from the rootProcesses argument that is in the form of
     * "processName1;processName2;..." with a trailing semi-colon.
     *
     * <pre>
     *   <ProcessInfo>
     *     <Process name="long_cat_viewer" uid="0" />
     *     ...
     *   </ProcessInfo>
     * </pre>
     */
    private void serializeProcessInfo(KXmlSerializer serializer, String rootProcesses)
            throws IOException {
        serializer.startTag(ns, PROCESS_INFO_TAG);

        if (rootProcesses == null) {
            rootProcesses = "";
        }

        String[] processNames = rootProcesses.split(PROCESS_DELIM);
        for (String processName : processNames) {
            processName = processName.trim();
            if (processName.length() > 0) {
                serializer.startTag(ns, PROCESS_TAG);
                serializer.attribute(ns, "name", processName);
                serializer.attribute(ns, "uid", "0");
                serializer.endTag(ns, PROCESS_TAG);
            }
        }
        serializer.endTag(ns, PROCESS_INFO_TAG);
    }

    /**
     * Prints XML data in two level hierarchy.
     * It parses a string from the root argument that is in the form of
     * "element1-delimiter-element2-delimiter..." with a trailing delimiter
     *
     * <pre>
     *   <infoTag>
     *     <elementTag name="element1" />
     *     ...
     *   </infoTag>
     * </pre>
     */
    private void serializeSimpleInfo(KXmlSerializer serializer, String root,
            String infoTag, String elementTag, String delimiter)
            throws IOException {
        serializer.startTag(ns, infoTag);

        if (root == null) {
            root = "";
        }

        String[] elemNames = root.split(delimiter);
        for (String elemName : elemNames) {
            elemName = elemName.trim();
            if (elemName.length() > 0) {
                serializer.startTag(ns, elementTag);
                serializer.attribute(ns, "name", elemName);
                serializer.endTag(ns, elementTag);
            }
        }
        serializer.endTag(ns, infoTag);
    }

    /**
     * Prints XML data listing available OpenGL Compressed Texture Formats.
     *
     * <pre>
     *   <OpenGLCompressedTextureFormatsInfo>
     *     <TextureFormat name="abc" />
     *     ...
     *   </OpenGLCompressedTextureFormatsInfo>
     * </pre>
     */
    private void serializeOpenGLCompressedTextureFormatsInfo(KXmlSerializer serializer,
            String root) throws IOException {
        serializeSimpleInfo(serializer, root, OPENGL_TEXTURE_FORMATS_INFO_TAG,
                OPENGL_TEXTURE_FORMAT_TAG,
                OPENGL_TEXTURE_FORMAT_DELIM);
    }

    /**
     * Prints XML data listing available system libraries.
     * It parses a string from the rootLibraries argument that is in the form of
     * "libName1;libName2;..." with a trailing semi-colon.
     *
     * <pre>
     *   <SystemLibrariesInfo>
     *     <Library name="a.b.c" />
     *     ...
     *   </SystemLibrariesInfo>
     * </pre>
     */
    private void serializeSystemLibrariesInfo(KXmlSerializer serializer, String rootLibraries)
            throws IOException {
        serializeSimpleInfo(serializer, rootLibraries, SYSLIB_INFO_TAG, SYSLIB_TAG, SYSLIB_DELIM);
    }

    /**
     * Populates this class with package result data parsed from XML.
     *
     * @param parser the {@link XmlPullParser}. Expected to be pointing at start
     *            of a {@link #TAG}
     */
    @Override
    void parse(XmlPullParser parser) throws XmlPullParserException, IOException {
        if (!parser.getName().equals(TAG)) {
            throw new XmlPullParserException(String.format(
                    "invalid XML: Expected %s tag but received %s", TAG, parser.getName()));
        }
        int eventType = parser.getEventType();
        while (eventType != XmlPullParser.END_DOCUMENT) {
            if (eventType == XmlPullParser.START_TAG) {
                if (parser.getName().equals(SCREEN_TAG) ||
                        parser.getName().equals(PHONE_TAG) ||
                        parser.getName().equals(BUILD_TAG)) {
                    addMetricsFromAttributes(parser);
                } else if (parser.getName().equals(FEATURE_INFO_TAG)) {
                    // store features into metrics map, in the same format as when collected from
                    // device
                    mMetrics.put(DeviceInfoConstants.FEATURES, parseFeatures(parser));
                } else if (parser.getName().equals(PROCESS_INFO_TAG)) {
                    // store processes into metrics map, in the same format as when collected from
                    // device
                    mMetrics.put(DeviceInfoConstants.PROCESSES, parseProcess(parser));
                } else if (parser.getName().equals(SYSLIB_INFO_TAG)) {
                    // store system libs into metrics map, in the same format as when collected from
                    // device
                    mMetrics.put(DeviceInfoConstants.SYS_LIBRARIES, parseSystemLibraries(parser));
                } else if (parser.getName().equals(OPENGL_TEXTURE_FORMATS_INFO_TAG)) {
                    // store OpenGL texture formats into metrics map, in the same format as when
                    // collected from device
                    mMetrics.put(DeviceInfoConstants.OPEN_GL_COMPRESSED_TEXTURE_FORMATS,
                            parseOpenGLCompressedTextureFormats(parser));
                }
            } else if (eventType == XmlPullParser.END_TAG && parser.getName().equals(TAG)) {
                return;
            }
            eventType = parser.next();
        }
    }

    /**
     * Parse process XML, and return its contents as a delimited String
     */
    private String parseProcess(XmlPullParser parser) throws XmlPullParserException, IOException {
        if (!parser.getName().equals(PROCESS_INFO_TAG)) {
            throw new XmlPullParserException(String.format(
                    "invalid XML: Expected %s tag but received %s", PROCESS_INFO_TAG,
                    parser.getName()));
        }
        StringBuilder processString = new StringBuilder();
        int eventType = parser.getEventType();
        while (eventType != XmlPullParser.END_DOCUMENT) {
            if (eventType == XmlPullParser.START_TAG && parser.getName().equals(PROCESS_TAG)) {
                processString.append(getAttribute(parser, "name"));
                processString.append(PROCESS_DELIM);
            } else if (eventType == XmlPullParser.END_TAG && parser.getName().equals(
                    PROCESS_INFO_TAG)) {
                return processString.toString();
            }
            eventType = parser.next();
        }
        return processString.toString();
    }

    /**
     * Parse feature XML, and return its contents as a delimited String
     */
    private String parseFeatures(XmlPullParser parser) throws XmlPullParserException, IOException {
        if (!parser.getName().equals(FEATURE_INFO_TAG)) {
            throw new XmlPullParserException(String.format(
                    "invalid XML: Expected %s tag but received %s", FEATURE_INFO_TAG,
                    parser.getName()));
        }
        StringBuilder featureString = new StringBuilder();
        int eventType = parser.getEventType();
        while (eventType != XmlPullParser.END_DOCUMENT) {
            if (eventType == XmlPullParser.START_TAG && parser.getName().equals(FEATURE_TAG)) {
                featureString.append(getAttribute(parser, "name"));
                featureString.append(FEATURE_ATTR_DELIM);
                featureString.append(getAttribute(parser, "type"));
                featureString.append(FEATURE_ATTR_DELIM);
                featureString.append(getAttribute(parser, "available"));
                featureString.append(FEATURE_DELIM);
            } else if (eventType == XmlPullParser.END_TAG
                    && parser.getName().equals(FEATURE_INFO_TAG)) {
                return featureString.toString();
            }
            eventType = parser.next();
        }
        return featureString.toString();

    }

    /**
     * Parse two-level hierarchy XML, and return its contents as a delimited String
     */
    private String parseSimpleInfo(XmlPullParser parser, String infoTag, String elementTag,
            String delimiter) throws XmlPullParserException, IOException {
        if (!parser.getName().equals(infoTag)) {
            throw new XmlPullParserException(String.format(
                    "invalid XML: Expected %s tag but received %s", infoTag,
                    parser.getName()));
        }
        StringBuilder result = new StringBuilder();
        int eventType = parser.getEventType();
        while (eventType != XmlPullParser.END_DOCUMENT) {
            if (eventType == XmlPullParser.START_TAG && parser.getName().equals(elementTag)) {
                result.append(getAttribute(parser, "name"));
                result.append(delimiter);
            } else if (eventType == XmlPullParser.END_TAG && parser.getName().equals(
                    infoTag)) {
                return result.toString();
            }
            eventType = parser.next();
        }
        return result.toString();
    }

    /**
     * Parse JavaLibrariesInfo XML, and return its contents as a delimited String
     */
    private String parseOpenGLCompressedTextureFormats(XmlPullParser parser)
            throws XmlPullParserException, IOException {
        return parseSimpleInfo(parser, OPENGL_TEXTURE_FORMATS_INFO_TAG, OPENGL_TEXTURE_FORMAT_TAG,
                OPENGL_TEXTURE_FORMAT_DELIM);
    }

    /**
     * Parse JavaLibrariesInfo XML, and return its contents as a delimited String
     */
    private String parseSystemLibraries(XmlPullParser parser)
            throws XmlPullParserException, IOException {
        return parseSimpleInfo(parser, SYSLIB_INFO_TAG, SYSLIB_TAG, SYSLIB_DELIM);
    }

    /**
     * Adds all attributes from the current XML tag to metrics as name-value pairs
     */
    private void addMetricsFromAttributes(XmlPullParser parser) {
        int attrCount = parser.getAttributeCount();
        for (int i = 0; i < attrCount; i++) {
            mMetrics.put(parser.getAttributeName(i), parser.getAttributeValue(i));
        }
    }

    /**
     * Populate the device info metrics with values collected from device.
     * <p/>
     * Check that the provided device info metrics are consistent with the currently stored metrics.
     * If any inconsistencies occur, logs errors and stores error messages in the metrics map
     *
     * @param runResult
     */
    public void populateMetrics(Map<String, String> metrics) {
        if (mMetrics.isEmpty()) {
            // no special processing needed, no existing metrics
            mMetrics.putAll(metrics);
            return;
        }
        Map<String, String> metricsCopy = new HashMap<String, String>(
                metrics);
        // add values for metrics that might be different across runs
        combineMetrics(metricsCopy, DeviceInfoConstants.PHONE_NUMBER, DeviceInfoConstants.IMSI,
                DeviceInfoConstants.IMSI, DeviceInfoConstants.SERIAL_NUMBER);

        // ensure all the metrics we expect to be identical actually are
        checkMetrics(metricsCopy, DeviceInfoConstants.BUILD_FINGERPRINT,
                DeviceInfoConstants.BUILD_MODEL, DeviceInfoConstants.BUILD_BRAND,
                DeviceInfoConstants.BUILD_MANUFACTURER, DeviceInfoConstants.BUILD_BOARD,
                DeviceInfoConstants.BUILD_DEVICE, DeviceInfoConstants.PRODUCT_NAME,
                DeviceInfoConstants.BUILD_ABI, DeviceInfoConstants.BUILD_ABI2,
                DeviceInfoConstants.SCREEN_SIZE);
    }

    private void combineMetrics(Map<String, String> metrics, String... keysToCombine) {
        for (String combineKey : keysToCombine) {
            String currentKeyValue = mMetrics.get(combineKey);
            String valueToAdd = metrics.remove(combineKey);
            if (valueToAdd != null) {
                if (currentKeyValue == null) {
                    // strange - no existing value. Can occur during unit testing
                    mMetrics.put(combineKey, valueToAdd);
                } else if (!currentKeyValue.equals(valueToAdd)) {
                    // new value! store a comma separated list
                    valueToAdd = String.format("%s,%s", currentKeyValue, valueToAdd);
                    mMetrics.put(combineKey, valueToAdd);
                } else {
                    // ignore, current value is same as existing
                }

            } else {
                CLog.d("Missing metric %s", combineKey);
            }
        }
    }

    private void checkMetrics(Map<String, String> metrics, String... keysToCheck) {
        Set<String> keyCheckSet = new HashSet<String>();
        Collections.addAll(keyCheckSet, keysToCheck);
        for (Map.Entry<String, String> metricEntry : metrics.entrySet()) {
            String currentValue = mMetrics.get(metricEntry.getKey());
            if (keyCheckSet.contains(metricEntry.getKey()) && currentValue != null
                    && !metricEntry.getValue().equals(currentValue)) {
                CLog.e("Inconsistent info collected from devices. "
                        + "Current result has %s='%s', Received '%s'. Are you sharding or " +
                        "resuming a test run across different devices and/or builds?",
                        metricEntry.getKey(), currentValue, metricEntry.getValue());
                mMetrics.put(metricEntry.getKey(),
                        String.format("ERROR: Inconsistent results: %s, %s",
                                metricEntry.getValue(), currentValue));
            } else {
                mMetrics.put(metricEntry.getKey(), metricEntry.getValue());
            }
        }
    }

    /**
     * Return the currently stored metrics.
     * <p/>
     * Exposed for unit testing.
     */
    Map<String, String> getMetrics() {
        return mMetrics;
    }
}
