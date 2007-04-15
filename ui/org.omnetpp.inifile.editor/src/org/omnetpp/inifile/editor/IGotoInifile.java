package org.omnetpp.inifile.editor;

/**
 * Interface to allow selection of an inifile entry or section.
 * InifileEditor implements this.
 * 
 * @author andras
 */
public interface IGotoInifile {
        enum Mode {AUTO, FORM, TEXT};
    
        /**
         * Opens the editor and shows the specified section. 
         * @param section  may be null (to mean deselect) 
         */
        public void gotoSection(String section, Mode mode);

        /**
         * Opens the editor and shows the specified inifile key. 
         * @param section  may be null (to mean deselect)
         * @param key  may be null (to mean deselect)
         */
        public void gotoEntry(String section, String key, Mode mode);
}
