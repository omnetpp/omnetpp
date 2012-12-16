package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.cdt.core.dom.ast.cpp.ICPPUsingDirective;
import org.eclipse.cdt.core.index.IIndexFile;
import org.eclipse.cdt.core.index.IIndexFileLocation;
import org.eclipse.cdt.core.index.IIndexMacro;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent.InclusionKind;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContentProvider;
import org.eclipse.core.resources.IFile;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.cache.Index.IndexFile;
import org.omnetpp.cdt.cache.Index.IndexInclude;
import org.omnetpp.common.util.Pair;

/**
 * This class provides the preprocessor with the content of source files.
 * The content can be either a character array or an index structure.
 * <p>
 * If an include file is requested more than once for a translation unit,
 * the file content is returned only for the first query, subsequent queries
 * will receive a skipped content. Here we assume that all header files contains
 * the necessary top level #ifdefs to prevent their content included twice.
 * This improves scanning time of large projects tremendously.
 *
 * @author tomi
 */
@SuppressWarnings("restriction")
class FileContentProvider extends InternalFileContentProvider {

    private Index index = Activator.getIndex();
    private FileCache fileCache = Activator.getFileCache();

    private IFile translationUnit;

    // files included into the current translation unit
    private Set<String> includedFiles = new HashSet<String>();

    private IMacroValueProvider currentMacroValues;

    static interface IMacroValueProvider {
        char[] getMacroValue(String name);
    }

    public FileContentProvider(IFile translationUnit) {
        this.translationUnit = translationUnit;
    }

    public void setMacroValueProvider(IMacroValueProvider provider) {
        this.currentMacroValues = provider;
    }

    public IFile getTranslationUnit() {
        return translationUnit;
    }

    public InternalFileContent getContentForTranslationUnit() {
        return getFileContent(translationUnit.getLocation().toOSString());
    }

    @Override
    public InternalFileContent getContentForInclusion(String path) {
        if (includedFiles.contains(path))
            return new InternalFileContent(path, InclusionKind.SKIP_FILE);

        InternalFileContent content = getFileContent(path);

        if (content != null)
            includedFiles.add(path);

        return content;
    }

    @Override
    public InternalFileContent getContentForInclusion(IIndexFileLocation ifl, String astPath) {
        throw new UnsupportedOperationException();
    }

    private InternalFileContent getFileContent(String path) {
        // check index
        IndexFile indexFile = index.resolve(path);
        if (indexFile != null) {
            try {
                Map<String,char[]> macroValues = new HashMap<String,char[]>();
                List<IIndexFile> files= new ArrayList<IIndexFile>();
                List<IIndexMacro> macros= new ArrayList<IIndexMacro>();
                Set<String> processedFiles= new HashSet<String>();
                collectFileContent(indexFile, processedFiles, files, macros, macroValues, false);
                // add included files only, if no exception was thrown
                for (String filename : processedFiles) {
                    if (!includedFiles.contains(filename))
                        includedFiles.add(filename);
                }
                return new InternalFileContent(path, macros, Collections.<ICPPUsingDirective>emptyList(), files);
            } catch (IncompatibleMacroValuesException e) {
                // TODO message dialog?
                //Activator.logError(e);
                //Debug.println(e.getMessage());
            }
        }

        // check file cache
        return fileCache.getFileContent(path);
    }

    class IncompatibleMacroValuesException extends Exception {
        private static final long serialVersionUID = 1L;

        public IncompatibleMacroValuesException() {
            super();
        }

        public IncompatibleMacroValuesException(String message) {
            super(message);
        }
    }

    private void collectFileContent(IndexFile file, Set<String> processedFiles, List<IIndexFile> files,
                                    List<IIndexMacro> macros, Map<String,char[]> macroValues, boolean checkIncluded)
        throws IncompatibleMacroValuesException
    {
        if (!processedFiles.add(file.getPath()) || (checkIncluded && includedFiles.contains(file.getPath()))) {
            return;
        }

        files.add(file);

        for (Object d : file.getPreprocessingDirectives()) {
            if (d instanceof Pair) {
                if (currentMacroValues != null) {
                    @SuppressWarnings("unchecked")
                    Pair<String,char[]> ref = (Pair<String,char[]>)d;
                    String name = ref.first;
                    char[] expectedValue = ref.second;
                    char[] foundValue = macroValues.containsKey(name) ? macroValues.get(name) :currentMacroValues.getMacroValue(name);
                    if (!Arrays.equals(expectedValue, foundValue))
                        throw new IncompatibleMacroValuesException(String.format("file: %s macro: %s expected: %s found: %s",
                                file.getPath(), name, expectedValue != null ? new String(expectedValue) : null, foundValue != null ? new String(foundValue) : foundValue));
                }
            }
            else if (d instanceof IIndexMacro) {
                IIndexMacro macro = (IIndexMacro)d;
                char[] expansion = macro.getExpansion();
                macros.add(macro);
                macroValues.put(macro.getName(), expansion);
            } else if (d instanceof IndexInclude) {
                IndexFile includedFile = index.resolve(((IndexInclude) d).getPath());
                if (includedFile != null) {
                    collectFileContent(includedFile, processedFiles, files, macros, macroValues, true);
                }
            }
        }
    }
}
