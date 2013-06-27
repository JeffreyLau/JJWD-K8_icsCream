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

# Licenses #

The Android Open Source Project uses a few [open source initiative](http://www.opensource.org/) 
approved open source licenses for our software.

## Android Open Source Project license ##

The preferred license for the Android Open Source Project is the [Apache 
Software License, 2.0](http://www.apache.org/licenses/LICENSE-2.0) ("Apache 2.0"), 
and the majority of the Android software is licensed
with Apache 2.0. While the project will strive to adhere to the preferred
license, there may be exceptions which will be handled on a case-by-case
basis. For example, the Linux kernel patches are under the GPLv2 license with
system exceptions, which can be found on [kernel.org](http://www.kernel.org/pub/linux/kernel/COPYING).

## Contributor License Grants ##

All *individual* contributors (that is, contributors making contributions
only on their own behalf) of ideas, code, or documentation to the Android Open
Source Project will be required to complete, sign, and submit an [Individual
Contributor License Grant](cla-individual.html). The grant can be executed online through the
[code review tool](https://review.source.android.com/#settings,agreements). 
The grant clearly defines the terms under which intellectual
property has been contributed to the Android Open Source Project. This license
is for your protection as a contributor as well as the protection of the
project; it does not change your rights to use your own contributions for any
other purpose.

For a *corporation* (or other entity) that has assigned employees to
work on the Android Open Source Project, a [Corporate
Contributor License Grant](cla-corporate.html) is available. 
This version of the grant allows a
corporation to authorize contributions submitted by its designated employees
and to grant copyright and patent licenses. Note that a Corporate Contributor
License Grant does not remove the need for any developer to sign their own
Individual Contributor License Grant as an individual, to cover any of their
contributions which are *not* owned by the corporation signing the
Corporate Contributor License Grant.

Please note that we based our grants on the ones that the 
[Apache Software Foundation](http://www.apache.org) uses, which can
be found on [the Apache web site](http://www.apache.org/licenses/).

## Why Apache Software License? ##

We are sometimes asked why Apache Software License 2.0 is the preferred
license for Android. For userspace (that is, non-kernel) software, we do in
fact prefer ASL2.0 (and similar licenses like BSD, MIT, etc.) over other
licenses such as LGPL.

Android is about freedom and choice. The purpose of Android is promote
openness in the mobile world, but we don't believe it's possible to predict or
dictate all the uses to which people will want to put our software. So, while
we encourage everyone to make devices that are open and modifiable, we don't
believe it is our place to force them to do so. Using LGPL libraries would
often force them to do so.

Here are some of our specific concerns:

- LGPL (in simplified terms) requires either: shipping of source to the
application; a written offer for source; or linking the LGPL-ed library
dynamically and allowing users to manually upgrade or replace the library.
Since Android software is typically shipped in the form of a static system
image, complying with these requirements ends up restricting OEMs' designs.
(For instance, it's difficult for a user to replace a library on read-only
flash storage.)

- LGPL requires allowance of customer modification and reverse
engineering for debugging those modifications.  Most device makers do
not want to have to be bound by these terms, so to minimize the burden on
these companies we minimize usage of LGPL software in userspace.</li>

- Historically, LGPL libraries have been the source of a large number
of compliance problems for downstream device makers and application
developers. Educating engineers on these issues is difficult and slow-going,
unfortunately. It's critical to Android's success that it be as easy as
possible for device makers to comply with the licenses.  Given the
difficulties with complying with LGPL in the past, it is most prudent to
simply not use LGPL libraries if we can avoid it.

The issues discussed above are our reasons for preferring ASL2.0 for
our own code. They aren't criticisms of LGPL or other licenses. We do
feel strongly on this topic, even to the point where we've gone out of our
way to make sure as much code as possible is ASL2.0. However, we love all free
and open source licenses, and respect others' opinions and preferences. We've
simply decided that ASL2.0 is the right license for our goals.

