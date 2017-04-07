package org.omnetpp.scave.export;

public class FileFormats {


    private static IGraphicalExportFileFormat[] formats;

    static {
        formats = new IGraphicalExportFileFormat[4];
        formats[0] = new PngFormat();
        formats[1] = new JpgFormat();
        formats[2] = new EpsFormat();
        formats[3] = new SvgFormat();
    }

    public static IGraphicalExportFileFormat[] getAll() {
        return formats;
    }
}
