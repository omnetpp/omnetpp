package org.omnetpp.scave.export;

/**
 * List of graphical export formats.
 *
 * @author andras
 */
public class GraphicalExportFileFormats {

    public static class FileFormat {
        private String name;
        private String description;

        public String getName() {
            return name;
        }
        public String getDescription() {
            return description;
        }
        public FileFormat(String name, String description) {
            super();
            this.name = name;
            this.description = description;
        }
    }

    private static FileFormat[] formats = new FileFormat[] {
            new FileFormat("svg", "Scalable Vector Graphics"),
            new FileFormat("eps", "Encapsulated Postscript"),
            new FileFormat("pdf", "Portable Document Format"),
            new FileFormat("png", "Portable Network Graphics"),
            new FileFormat("jpg", "JPEG Image (lossy)"),
    };

    public static FileFormat[] getAll() {
        return formats;
    }
}
