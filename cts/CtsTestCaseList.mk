# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CTS_SECURITY_APPS_LIST := \
	CtsAppAccessData \
	CtsAppWithData \
	CtsInstrumentationAppDiffCert \
	CtsPermissionDeclareApp \
	CtsSharedUidInstall \
	CtsSharedUidInstallDiffCert \
	CtsSimpleAppInstall \
	CtsSimpleAppInstallDiffCert \
	CtsTargetInstrumentationApp \
	CtsUsePermissionDiffCert \
	CtsMonkeyApp \
	CtsMonkeyApp2 \

# These test cases will be analyzed by the CTS API coverage tools. 
CTS_COVERAGE_TEST_CASE_LIST := \
	CtsAccelerationTestCases \
	CtsAccelerationTestStubs \
	CtsAccessibilityServiceTestCases \
	CtsAccountManagerTestCases \
	CtsAdminTestCases \
	CtsAnimationTestCases \
	CtsAppTestCases \
	CtsBluetoothTestCases \
	CtsContentTestCases \
	CtsDatabaseTestCases \
	CtsDelegatingAccessibilityService \
	CtsDeviceAdmin \
	CtsDpiTestCases \
	CtsDpiTestCases2 \
	CtsDrmTestCases \
	CtsExampleTestCases \
	CtsGestureTestCases \
	CtsGraphicsTestCases \
	CtsGraphics2TestCases \
	CtsHardwareTestCases \
	CtsHoloTestCases \
	CtsJniTestCases \
	CtsLocationTestCases \
	CtsMediaStressTestCases \
	CtsMediaTestCases \
	CtsNdefTestCases \
	CtsNetTestCases \
	CtsOpenGlPerfTestCases \
	CtsOsTestCases \
	CtsPermissionTestCases \
	CtsPermission2TestCases \
	CtsPreferenceTestCases \
	CtsPreference2TestCases \
	CtsProviderTestCases \
	CtsRenderscriptTestCases \
	CtsSaxTestCases \
	CtsSecurityTestCases \
	CtsSpeechTestCases \
	CtsTelephonyTestCases \
	CtsTestStubs \
	CtsTextTestCases \
	CtsTextureViewTestCases \
	CtsUtilTestCases \
	CtsViewTestCases \
	CtsWebkitTestCases \
	CtsWidgetTestCases \
	SignatureTest \
	TestDeviceSetup \
	$(CTS_SECURITY_APPS_LIST)

CTS_TEST_CASE_LIST := \
	com.replica.replicaisland \
	ApiDemos \
	ApiDemosReferenceTest \
	$(CTS_COVERAGE_TEST_CASE_LIST) \

CTS_NATIVE_EXES := \
	CtsNativeMediaTestCases

CTS_TEST_CASES := $(call cts-get-native-paths,$(CTS_NATIVE_EXES))

CTS_TEST_XMLS := $(call cts-get-native-xmls,$(CTS_NATIVE_EXES))

# The following files will be placed in the tools directory of the CTS distribution
CTS_TOOLS_LIST :=
