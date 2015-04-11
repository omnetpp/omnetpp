/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.test.unit.common;

import junit.framework.TestCase;

import org.junit.Test;
import org.omnetpp.common.util.LicenseUtils;

public class LicenseUtilsTest extends TestCase {

    @Test
    public void testLicenses() {
        String[] licenses = LicenseUtils.getLicenses();
        for (String license : licenses) {
            // there must be a license notice for each license
            String licenseNotice = LicenseUtils.getLicenseNotice(license);
            assertNotNull(licenseNotice);

            if (license.equals(LicenseUtils.NONE))
                assertTrue(licenseNotice.equals(""));
            else
                assertTrue(!licenseNotice.equals(""));

            // should not throw error
            LicenseUtils.getBannerComment(license, "//");
        }
    }
}
