/*
 * Copyright (C) 2012 The Android Open Source Project
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

import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.provider.MediaStore;
import android.provider.MediaStore.MediaColumns;
import android.test.AndroidTestCase;

public class MediaStore_FilesTest extends AndroidTestCase {

    private ContentResolver mResolver;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mResolver = mContext.getContentResolver();
    }

    public void testGetContentUri() {
        String volumeName = MediaStoreAudioTestHelper.EXTERNAL_VOLUME_NAME;
        Uri allFilesUri = MediaStore.Files.getContentUri(volumeName);

        // Get the current file count. We will check if this increases after
        // adding a file to the provider.
        int fileCount = getFileCount(allFilesUri);

        // Check that inserting empty values causes an exception.
        ContentValues values = new ContentValues();
        try {
            mResolver.insert(allFilesUri, values);
            fail("Should throw an exception");
        } catch (IllegalArgumentException e) {
            // Expecting an exception
        }

        // Add a path for a file and check that the returned uri appends a
        // path properly.
        String dataPath = "does_not_really_exist.txt";
        values.put(MediaColumns.DATA, dataPath);
        Uri fileUri = mResolver.insert(allFilesUri, values);
        long fileId = ContentUris.parseId(fileUri);
        assertEquals(fileUri, ContentUris.withAppendedId(allFilesUri, fileId));

        // Check that getContentUri with the file id produces the same url
        Uri rowUri = MediaStore.Files.getContentUri(volumeName, fileId);
        assertEquals(fileUri, rowUri);

        // Check that the file count has increased.
        int newFileCount = getFileCount(allFilesUri);
        assertEquals(fileCount + 1, newFileCount);

        // Check that the path we inserted was stored properly.
        assertStringColumn(fileUri, MediaColumns.DATA, dataPath);

        // Update the path and check that the database changed.
        String updatedPath = "still_does_not_exist.txt";
        values.put(MediaColumns.DATA, updatedPath);
        assertEquals(1, mResolver.update(fileUri, values, null, null));
        assertStringColumn(fileUri, MediaColumns.DATA, updatedPath);

        // Delete the file and observe that the file count decreased.
        assertEquals(1, mResolver.delete(fileUri, null, null));
        assertEquals(fileCount, getFileCount(allFilesUri));

        // Make sure the deleted file is not returned by the cursor.
        Cursor cursor = mResolver.query(fileUri, null, null, null, null);
        try {
            assertFalse(cursor.moveToNext());
        } finally {
            cursor.close();
        }
    }

    private int getFileCount(Uri uri) {
        Cursor cursor = mResolver.query(uri, null, null, null, null);
        try {
            return cursor.getCount();
        } finally {
            cursor.close();
        }
    }

    private void assertStringColumn(Uri fileUri, String columnName, String expectedValue) {
        Cursor cursor = mResolver.query(fileUri, null, null, null, null);
        try {
            assertTrue(cursor.moveToNext());
            int index = cursor.getColumnIndexOrThrow(columnName);
            assertEquals(expectedValue, cursor.getString(index));
        } finally {
            cursor.close();
        }
    }
}
