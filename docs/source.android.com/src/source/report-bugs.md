<!--
   Copyright 2010 The Android Open Source Project 

   Licensed under the Apache License, Version 2.0 (the "License"); 
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
-->

# Report Bugs #

Thanks for your interest in Android! One of the best ways you can help us
improve Android is to let us know about any problems you find with it.

First, though: if you think you've found a security vulnerability,
*please don't use the forms below*. Using the public forms below may
allow anyone to see your report, which may put users at risk until the bug is
fixed. Instead, please report security bugs to our security team by emailing
<a href="mailto:security@android.com">security@android.com</a>.  We believe in
responsible disclosure of security vulnerabilities, and will give you proper
attribution for any issues you report.

Here's how to report non-security bugs:

- [Search for your bug](http://code.google.com/p/android/issues/advsearch) to see if anyone has already reported it.

- If you find your issue and it's important to you, star it! That's how we know which bugs are most important to fix.

- If no one's reported your bug, file the bug. You can use one of these templates:

    - [Bug in your Device](http://code.google.com/p/android/issues/entry?template=User%20bug%20report) - use this if you are a user reporting a bug in a device you own

    - [Bug in the Software](http://code.google.com/p/android/issues/entry?template=Developer%20bug%20report) - use this if you found a bug in the course of developing an app

    - [Feature Request](http://code.google.com/p/android/issues/entry?template=Feature%20request) - use this for a feature you'd like to see in a future verison

Please note that we can't guarantee that any particular bug can be fixed in
any particular release. To see what happens to your bug once you report it,
read [Life of a Bug](life-of-a-bug.html).

In general, please put as much info in bugs as you can. Just a one liner
telling us something isn't working is usually useless, and will probably be
closed without any action. The more detail you provide, the more likely your
issue is to be resolved. Below, there are some examples of a good bug report
and a poor bug report.

## A Poor Bug Report ##
    Title: Error message

    When running Eclipse I get an "Internal Error" that says "See the .log file for more details".

    Steps to reproduce:
    Happens when "Object o = null". Doesn't happen when changed to "Object o".

    Expected results:
    I wouldn't get the error message--would work with Object o = null.

    Observed results:
    See above.

This is a poor bug report because it doesn't provide any context for the
issue; is it a problem in the Dalvik virtual machine, the core framework, or
something else? It also doesn't provide any code or hint on how to reproduce
it. In other words, this bug report doesn't provide enough information for
anyone to take action on, so it would be ignored.

## A Good Bug Report ##

    Title: Stepping over "Object o = null" causes Eclipse "Internal Error"

    Interesting bug, while using Eclipse 3.3.1.1 with m37a of android and the following code:

    package com.saville.android;

    import android.app.Activity;
    import android.os.Bundle;
    import android.util.Log;

    public class TestObjectNull extends Activity {
        /** Called when the activity is first created. */
        @Override
        public void onCreate(Bundle icicle) {
            super.onCreate(icicle);
            setContentView(R.layout.main);
    
            Object o = null;
    
            o = "hi";
    
            Log.v(TAG, "o=" + o);
        }

        static final String TAG = "TestObjectNull";
    }

    Eclipse indicates an "Internal Error" with "See the .log file for more
    details" and then asks if I want to exit the workbench. This occurs when I
    place a break point on "setContentView(R.layout.main);" and then single
    step over "Object o = null;"

    If I change "Object o = null;" to "Object o" all is well.

    The last lines of the .log file are:

    !ENTRY org.eclipse.core.jobs 4 2 2008-01-01 13:04:15.825
    !MESSAGE An internal error occurred during: "has children update".
    !STACK 0
    java.lang.InternalError: Invalid signature: "<null>"
            at
    org.eclipse.jdi.internal.TypeImpl.signatureToTag(TypeImpl.java:307)
            at
    org.eclipse.jdi.internal.LocalVariableImpl.tag(LocalVariableImpl.java:185)
            at
    org.eclipse.jdi.internal.StackFrameImpl.getValues(StackFrameImpl.java:128)
            at
    org.eclipse.jdi.internal.StackFrameImpl.getValue(StackFrameImpl.java:73)
            at
    org.eclipse.jdt.internal.debug.core.model.JDILocalVariable.retrieveValue(JDILocalVariable.java:57)
            at
    org.eclipse.jdt.internal.debug.core.model.JDIVariable.getCurrentValue(JDIVariable.java:66)
            at
    org.eclipse.jdt.internal.debug.core.model.JDIVariable.getValue(JDIVariable.java:88)
            at
    org.eclipse.debug.internal.ui.model.elements.VariableContentProvider.hasChildren(VariableContentProvider.java:62)
            at
    org.eclipse.jdt.internal.debug.ui.variables.JavaVariableContentProvider.hasChildren(JavaVariableContentProvider.java:73)
            at
    org.eclipse.debug.internal.ui.model.elements.ElementContentProvider.updateHasChildren(ElementContentProvider.java:223)
            at
    org.eclipse.debug.internal.ui.model.elements.ElementContentProvider$3.run(ElementContentProvider.java:200)
            at org.eclipse.core.internal.jobs.Worker.run(Worker.java:55)

