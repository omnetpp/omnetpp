/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.apache.commons.lang3.ArrayUtils;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;

/**
 * Licenses
 *
 * @author Andras
 */
public class LicenseUtils {
    public static final String GPL = "GPL";
    public static final String LGPL= "LGPL";
    public static final String BSD = "BSD";
    public static final String CUSTOM = "custom";
    public static final String NONE = "none";

    protected static final String BSD_HEADER =
        "Permission to use, copy, modify, and/or distribute this software for any\n" +
        "purpose with or without fee is hereby granted, provided that the above\n" +
        "copyright notice and this permission notice appear in all copies.\n" +
        "\n" +
        "THE SOFTWARE IS PROVIDED \"AS IS\" AND THE AUTHOR DISCLAIMS ALL WARRANTIES\n" +
        "WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF\n" +
        "MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR\n" +
        "ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES\n" +
        "WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN\n" +
        "ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF\n" +
        "OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.\n";

    protected static final String GNU_HEADER =
        "This program is free software: you can redistribute it and/or modify\n" +
        "it under the terms of the GNU @KIND@ Public License as published by\n" +
        "the Free Software Foundation, either version @VERSION@ of the License, or\n" +
        "(at your option) any later version.\n" +
        "\n" +
        "This program is distributed in the hope that it will be useful,\n" +
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" +
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" +
        "GNU @KIND@ Public License for more details.\n" +
        "\n" +
        "You should have received a copy of the GNU @KIND@ Public License\n" +
        "along with this program.  If not, see http://www.gnu.org/licenses/.\n";

    public static final String DEFAULT_CUSTOM_LICENSE_HEADER =
        "This program is property of its copyright holder. All rights reserved.";

    /**
     * Returns the list of recognized license codes.
     */
    public static String[] getLicenses() {
        if (IConstants.IS_COMMERCIAL)
            return new String[] {NONE,CUSTOM,GPL,LGPL,BSD};
        else
            return new String[] {GPL,LGPL,BSD};
    }

    /**
     * Returns true if the given string is a recognized license code; see getLicenses().
     */
    public static boolean isAcceptedLicense(String licenseCode) {
        return ArrayUtils.contains(getLicenses(), licenseCode);
    }

    /**
     * Returns the license code configured as default in the preferences.
     */
    public static String getDefaultLicense() {
        return CommonPlugin.getConfigurationPreferenceStore().getString(IConstants.PREF_DEFAULT_LICENSE);
    }

    /**
     * Returns the notice text for the given license code.
     */
    public static String getLicenseNotice(String licenseCode) {
        if (licenseCode.equals(GPL))
            return GNU_HEADER.replace("@KIND@", "General").replace("@VERSION@", "3");
        else if (licenseCode.equals(LGPL))
            return GNU_HEADER.replace("@KIND@", "Lesser General").replace("@VERSION@", "3");
        else if (licenseCode.equals(BSD))
            return BSD_HEADER;
        else if (licenseCode.equals(CUSTOM))
            return CommonPlugin.getConfigurationPreferenceStore().getString(IConstants.PREF_CUSTOM_LICENSE_HEADER);
        else if (licenseCode.equals(NONE))
            return "";
        else
            throw new IllegalArgumentException("unrecognized license identifier: "+ licenseCode);
    }

    /**
     * Returns the configured copyright line from the preferences.
     */
    public static String getCopyrightLine() {
        return CommonPlugin.getConfigurationPreferenceStore().getString(IConstants.PREF_COPYRIGHT_LINE);
    }

    /**
     * Returns a full comment, ready to be inserted to top of a source file.
     * Includes copyright line and license notice as well. commentChars
     * should be "//" for C++, NED and Java files.
     */
    public static String getBannerComment(String licenseCode, String commentChars) {
        String txt = getCopyrightLine().trim() + "\n\n" + getLicenseNotice(licenseCode).trim();
        txt = txt.trim();
        if (!txt.equals("")) {
            txt = "\n" + txt + "\n";
            txt = commentChars + txt.replace("\n", "\n" + commentChars + " ") + "\n\n";
        }
        return txt;
    }
}
