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

# Downloading the Source Tree #

## Installing Repo ##

Repo is a tool that makes it easier to work with Git in the context of Android. For more information about Repo, see [Version Control](version-control.html).

To install, initialize, and configure Repo, follow these steps:

 - Make sure you have a bin/ directory in your home directory, and that it is included in your path:

        $ mkdir ~/bin
        $ PATH=~/bin:$PATH

 - Download the Repo script and ensure it is executable:

        $ curl https://dl-ssl.google.com/dl/googlesource/git-repo/repo > ~/bin/repo
        $ chmod a+x ~/bin/repo

 - The SHA-1 checksum for repo is e1fd3bef059d152edf4d0522590725d317bc637f

## Initializing a Repo client ##

After installing Repo, set up your client to access the android source repository:

 - Create an empty directory to hold your working files.
 Give it any name you like:


        $ mkdir WORKING_DIRECTORY
        $ cd WORKING_DIRECTORY

 - Run `repo init` to bring down the latest version of Repo with all its most recent bug fixes.  You must specify a URL for the manifest, which specifies where the various repositories included in the Android source will be placed within your working directory.

        $ repo init -u https://android.googlesource.com/platform/manifest

    To check out a branch other than "master", specify it with -b:

        $ repo init -u https://android.googlesource.com/platform/manifest -b android-2.3.7_r1

 - When prompted, please configure Repo with your real name and email address.  To use the Gerrit code-review tool, you will need an email address that is connected with a [registered Google account](http://www.google.com/accounts).  Make sure this is a live address at which you can receive messages.  The name that you provide here will show up in attributions for your code submissions.

A successful initialization will end with a message stating that Repo is initialized in your working directory.  Your client directory should now contain a `.repo` directory where files such as the manifest will be kept.



## Getting the files ##

To pull down files to your working directory from the repositories as specified in the default manifest, run

    $ repo sync

The Android source files will be located in your working directory
under their project names. The initial sync operation will take
an hour or more to complete. For more about `repo sync` and other
Repo commands, see [Version Control](version-control.html).


## Verifying Git Tags ##

Load the following public key into your GnuPG key database. The key is used to sign annotated tags that represent releases.

    $ gpg --import

Copy and paste the key(s) below, then enter EOF (Ctrl-D) to end the input and process the keys.

    -----BEGIN PGP PUBLIC KEY BLOCK-----
    Version: GnuPG v1.4.2.2 (GNU/Linux)

    mQGiBEnnWD4RBACt9/h4v9xnnGDou13y3dvOx6/t43LPPIxeJ8eX9WB+8LLuROSV
    lFhpHawsVAcFlmi7f7jdSRF+OvtZL9ShPKdLfwBJMNkU66/TZmPewS4m782ndtw7
    8tR1cXb197Ob8kOfQB3A9yk2XZ4ei4ZC3i6wVdqHLRxABdncwu5hOF9KXwCgkxMD
    u4PVgChaAJzTYJ1EG+UYBIUEAJmfearb0qRAN7dEoff0FeXsEaUA6U90sEoVks0Z
    wNj96SA8BL+a1OoEUUfpMhiHyLuQSftxisJxTh+2QclzDviDyaTrkANjdYY7p2cq
    /HMdOY7LJlHaqtXmZxXjjtw5Uc2QG8UY8aziU3IE9nTjSwCXeJnuyvoizl9/I1S5
    jU5SA/9WwIps4SC84ielIXiGWEqq6i6/sk4I9q1YemZF2XVVKnmI1F4iCMtNKsR4
    MGSa1gA8s4iQbsKNWPgp7M3a51JCVCu6l/8zTpA+uUGapw4tWCp4o0dpIvDPBEa9
    b/aF/ygcR8mh5hgUfpF9IpXdknOsbKCvM9lSSfRciETykZc4wrRCVGhlIEFuZHJv
    aWQgT3BlbiBTb3VyY2UgUHJvamVjdCA8aW5pdGlhbC1jb250cmlidXRpb25AYW5k
    cm9pZC5jb20+iGAEExECACAFAknnWD4CGwMGCwkIBwMCBBUCCAMEFgIDAQIeAQIX
    gAAKCRDorT+BmrEOeNr+AJ42Xy6tEW7r3KzrJxnRX8mij9z8tgCdFfQYiHpYngkI
    2t09Ed+9Bm4gmEO5Ag0ESedYRBAIAKVW1JcMBWvV/0Bo9WiByJ9WJ5swMN36/vAl
    QN4mWRhfzDOk/Rosdb0csAO/l8Kz0gKQPOfObtyYjvI8JMC3rmi+LIvSUT9806Up
    hisyEmmHv6U8gUb/xHLIanXGxwhYzjgeuAXVCsv+EvoPIHbY4L/KvP5x+oCJIDbk
    C2b1TvVk9PryzmE4BPIQL/NtgR1oLWm/uWR9zRUFtBnE411aMAN3qnAHBBMZzKMX
    LWBGWE0znfRrnczI5p49i2YZJAjyX1P2WzmScK49CV82dzLo71MnrF6fj+Udtb5+
    OgTg7Cow+8PRaTkJEW5Y2JIZpnRUq0CYxAmHYX79EMKHDSThf/8AAwUIAJPWsB/M
    pK+KMs/s3r6nJrnYLTfdZhtmQXimpoDMJg1zxmL8UfNUKiQZ6esoAWtDgpqt7Y7s
    KZ8laHRARonte394hidZzM5nb6hQvpPjt2OlPRsyqVxw4c/KsjADtAuKW9/d8phb
    N8bTyOJo856qg4oOEzKG9eeF7oaZTYBy33BTL0408sEBxiMior6b8LrZrAhkqDjA
    vUXRwm/fFKgpsOysxC6xi553CxBUCH2omNV6Ka1LNMwzSp9ILz8jEGqmUtkBszwo
    G1S8fXgE0Lq3cdDM/GJ4QXP/p6LiwNF99faDMTV3+2SAOGvytOX6KjKVzKOSsfJQ
    hN0DlsIw8hqJc0WISQQYEQIACQUCSedYRAIbDAAKCRDorT+BmrEOeCUOAJ9qmR0l
    EXzeoxcdoafxqf6gZlJZlACgkWF7wi2YLW3Oa+jv2QSTlrx4KLM=
    =Wi5D
    -----END PGP PUBLIC KEY BLOCK-----

After importing the keys, you can verify any tag with

    $ git tag -v TAG_NAME


# Next: Build the code #

You now have a complete local copy of the Android codebase.  Continue on to [building](building.html)....
