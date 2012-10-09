package org.omnetpp.scave.model2;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Shell;

/**
 * Utility class for generating unique file names.
 *
 * @author tomi
 */
public class FilenameGenerator {

    public enum Overwrite {
        Yes, No, Ask
    }

    final File targetFolder;
    final String extension;
    Overwrite overWrite = Overwrite.Ask;
    Map<String,Integer> counts = new HashMap<String, Integer>();

    public FilenameGenerator(File folder, String extension, Overwrite overwrite) {
        this.targetFolder = folder;
        this.extension = extension;
        this.overWrite = overwrite;
    }

    public List<File> generateFilenames(List<String> baseNames, Shell parentShell) {
        List<File> files = new ArrayList<File>(baseNames.size());
        for (String name : baseNames) {
            Integer count = counts.get(name);
            counts.put(name, count == null ? 1 : count+1);
        }
        Iterator<Map.Entry<String,Integer>> entries = counts.entrySet().iterator();
        while (entries.hasNext()) {
            Map.Entry<String, Integer> entry = entries.next();
            if (entry.getValue() <= 1)
                entries.remove(); // start names without index
            else
                entry.setValue(1); // start names with _1
        }

        for (String baseName : baseNames) {
            String fileName = nextFileName(baseName);
            File file = checkIfExist(fileName, baseName, parentShell);
            files.add(file);
        }
        return files;
    }

    private String nextFileName(String baseName) {
        Integer count = counts.get(baseName);
        String fileName = count != null ? baseName + "_" + count : baseName;
        counts.put(baseName, count == null ? 1 : count+1);
        return fileName;
    }

    private File checkIfExist(String fileName, String baseName, Shell parentShell) {
        File file = new File(targetFolder, fileName + "." + extension);
        Overwrite overWrite = this.overWrite;
        if (overWrite == Overwrite.Ask && file.exists()) {
            overWrite = openConfirmDialog(file, parentShell);
        }

        if (overWrite == Overwrite.No) {
            while (file.exists()) {
                fileName = nextFileName(baseName);
                file = new File(targetFolder, fileName + "." + extension);
            }
        }

        return file;
    }

    private Overwrite openConfirmDialog(File file, Shell parentShell) {
        MessageDialog dialog = new MessageDialog(parentShell, "Overwrite files?", null/*image*/,
                String.format("The '%s' file already exists. Do you want to overwrite it?", file.getAbsolutePath()),
                MessageDialog.NONE, new String[] {"Yes", "No", "Yes to All", "No to All"}, -1);
        int result = dialog.open();
        if (result == SWT.DEFAULT)
            throw new OperationCanceledException();
        switch (result) {
        case 0: return Overwrite.Yes;
        case 1: return Overwrite.No;
        case 2: return overWrite = Overwrite.Yes;
        case 3: return overWrite = Overwrite.No;
        }
        Assert.isTrue(false);
        return Overwrite.No;
    }
}
