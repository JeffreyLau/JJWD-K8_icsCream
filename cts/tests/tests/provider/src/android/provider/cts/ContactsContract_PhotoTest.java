/*
 * Copyright (C) 2010 The Android Open Source Project
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

package android.provider.cts;

import com.android.cts.stub.R;

import android.content.ContentResolver;
import android.content.IContentProvider;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds.Photo;
import android.provider.ContactsContract.Contacts;
import android.provider.cts.ContactsContract_TestDataBuilder.TestContact;
import android.provider.cts.ContactsContract_TestDataBuilder.TestData;
import android.provider.cts.ContactsContract_TestDataBuilder.TestRawContact;
import android.test.InstrumentationTestCase;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class ContactsContract_PhotoTest extends InstrumentationTestCase {
    private ContactsContract_TestDataBuilder mBuilder;

    private static final byte[] EMPTY_TEST_PHOTO_DATA = "".getBytes();

    private ContentResolver mResolver;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mResolver = getInstrumentation().getTargetContext().getContentResolver();
        IContentProvider provider = mResolver.acquireProvider(ContactsContract.AUTHORITY);
        mBuilder = new ContactsContract_TestDataBuilder(provider);
    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
        mBuilder.cleanup();
    }

    public void testAddPhoto() throws Exception {
        TestRawContact rawContact = mBuilder.newRawContact().insert().load();
        TestContact contact = rawContact.getContact().load();

        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri()));
        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), true));
        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), false));

        TestData photoData = rawContact.newDataRow(Photo.CONTENT_ITEM_TYPE)
                .with(Photo.PHOTO, getTestPhotoData())
                .insert();

        photoData.load();
        photoData.assertColumn(Photo.RAW_CONTACT_ID, rawContact.getId());
        photoData.assertBlobColumnNotNull(Photo.PHOTO);

        assertPhotoStream(Contacts.openContactPhotoInputStream(mResolver, contact.getUri()));
        assertPhotoStream(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), true));
        assertPhotoStream(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), false));
    }

    public void testAddEmptyPhoto() throws Exception {
        TestRawContact rawContact = mBuilder.newRawContact().insert().load();
        TestContact contact = rawContact.getContact().load();

        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri()));
        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), true));
        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), false));

        TestData photoData = rawContact.newDataRow(Photo.CONTENT_ITEM_TYPE)
                .with(Photo.PHOTO, EMPTY_TEST_PHOTO_DATA)
                .insert();
        assertNotNull(photoData.load());

        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri()));
        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), true));
        assertNull(Contacts.openContactPhotoInputStream(mResolver, contact.getUri(), false));
    }

    private byte[] getTestPhotoData() {
        InputStream input = getInstrumentation().getTargetContext().getResources()
                .openRawResource(R.drawable.testimage);
        return readInputStreamFully(input);
    }

    protected byte[] readInputStreamFully(InputStream is) {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        byte[] buffer = new byte[10000];
        int count;
        try {
            while ((count = is.read(buffer)) != -1) {
                os.write(buffer, 0, count);
            }
            is.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        return os.toByteArray();
    }

    private void assertPhotoStream(InputStream photoStream) throws IOException {
        try {
            assertNotNull(photoStream);
            byte[] actualBytes = readInputStreamFully(photoStream);
            assertTrue(actualBytes.length > 0);
        } finally {
            if (photoStream != null) {
                photoStream.close();
            }
        }
    }
}

