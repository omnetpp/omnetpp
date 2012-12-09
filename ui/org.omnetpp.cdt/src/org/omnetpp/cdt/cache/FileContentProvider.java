package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
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

    // TODO set value
    private IMacroValueProvider currentMacroValues;

    static interface IMacroValueProvider {
        String getValue(String name);
    }

    public FileContentProvider(IFile translationUnit) {
        this.translationUnit = translationUnit;
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
        if (indexFile != null && (currentMacroValues == null || indexFile.isCompatibleWithMacroValues(currentMacroValues))) {
            try {
                List<IIndexFile> files= new ArrayList<IIndexFile>();
                List<IIndexMacro> macros= new ArrayList<IIndexMacro>();
                Set<String> processedFiles= new HashSet<String>();
                collectFileContent(indexFile, processedFiles, files, macros, false);
                // add included files only, if no exception was thrown
                for (String filename : processedFiles) {
                    if (!includedFiles.contains(filename))
                        includedFiles.add(filename);
                }
                return new InternalFileContent(path, macros, Collections.<ICPPUsingDirective>emptyList(), files);
            } catch (Exception e) {
                // TODO message dialog?
                Activator.logError(e);
            }
        }

        // check file cache
        return fileCache.getFileContent(path);
    }

    private void collectFileContent(IndexFile file, Set<String> processedFiles, List<IIndexFile> files, List<IIndexMacro> macros, boolean checkIncluded) {
        if (!processedFiles.add(file.getPath()) || (checkIncluded && includedFiles.contains(file.getPath()))) {
            return;
        }

        files.add(file);

        for (Object d : file.getPreprocessingDirectives()) {
            if (d instanceof IIndexMacro) {
                macros.add((IIndexMacro) d);
            } else if (d instanceof IndexInclude) {
                IndexFile includedFile = index.resolve(((IndexInclude) d).getPath());
                if (includedFile != null) {
                    // TODO check macro dependencies
                    collectFileContent(includedFile, processedFiles, files, macros, true);
                }
            }
        }
    }
}
