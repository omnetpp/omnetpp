package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.TreeSet;

import org.apache.commons.lang.ObjectUtils;
import org.apache.commons.lang.StringUtils;
import org.eclipse.cdt.core.dom.ILinkage;
import org.eclipse.cdt.core.dom.ast.DOMException;
import org.eclipse.cdt.core.dom.ast.IASTFileLocation;
import org.eclipse.cdt.core.dom.ast.IASTName;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIncludeStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorUndefStatement;
import org.eclipse.cdt.core.dom.ast.IMacroBinding;
import org.eclipse.cdt.core.dom.ast.IScope;
import org.eclipse.cdt.core.dom.ast.cpp.ICPPUsingDirective;
import org.eclipse.cdt.core.index.IIndexBinding;
import org.eclipse.cdt.core.index.IIndexFile;
import org.eclipse.cdt.core.index.IIndexFileLocation;
import org.eclipse.cdt.core.index.IIndexInclude;
import org.eclipse.cdt.core.index.IIndexMacro;
import org.eclipse.cdt.core.index.IIndexName;
import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.parser.util.CharArrayMap;
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
import org.eclipse.cdt.internal.core.index.IndexFileLocation;
import org.eclipse.cdt.utils.UNCPathConverter;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.cache.CachedFileContentProvider.IMacroValueProvider;
import org.omnetpp.cdt.cache.CachedFileContentProvider.ISourceFileChangeListener;
import org.omnetpp.common.Debug;

@SuppressWarnings("restriction")
public class Index {

    Map<String,IndexFile> files = new HashMap<String,IndexFile>();
    Indexer indexer = new Indexer();
    CachedFileContentProvider fileContentProvider;

    private ISourceFileChangeListener sourceFileChangeListener = new ISourceFileChangeListener() {
        public void sourceFileChanged(IResourceDelta delta) {
            Index.this.sourceFileChanged(delta);
        }
    };

    private ICProjectDescriptionListener projectDescriptionListener = new ICProjectDescriptionListener() {
        public void handleEvent(CProjectDescriptionEvent e) {
            projectDescriptionChanged(e);
        }
    };

    public Index(CachedFileContentProvider fileContentProvider) {
        this.fileContentProvider = fileContentProvider;
    }

    public void hookListeners() {
        Activator.getFileContentProvider().addFileChangeListener(sourceFileChangeListener);
        CoreModel.getDefault().addCProjectDescriptionListener(projectDescriptionListener, CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        Activator.getFileContentProvider().removeFileChangeListener(sourceFileChangeListener);
        CoreModel.getDefault().removeCProjectDescriptionListener(projectDescriptionListener);
    }

    private boolean purgeAll = false;
    private void sourceFileChanged(IResourceDelta delta) {
        if (purgeAll) {
            Debug.format("Index: source file changed, discarding index\n");
            Set<String> keys = new HashSet<String>(files.keySet());
            for (String key : keys)
                removeFromIndex(files.get(key));
        }
        else {
            IndexFile indexFile = resolve(delta.getResource().getLocation().toOSString());
            if (indexFile != null) {
                removeFromIndex(indexFile);
                Debug.format("Index: source file %s changed, discarding index\n", indexFile.absolutePath);
                int count = 1;
                for (IndexFile file : findIncludedBy(indexFile)) {
                    removeFromIndex(file);
                    count++;
                }
                Debug.format("  removed %d/%d files\n", count, count+files.size());
            }
        }
    }

    private void projectDescriptionChanged(CProjectDescriptionEvent e) {
        Debug.println("Index: project description changed, discarding index");
        clear();
    }

    public Indexer getIndexer() {
        return indexer;
    }

    public void clear() {
        files.clear();
    }

    public void removeFromIndex(IndexFile file) {
        files.remove(file.absolutePath);
        fileContentProvider.setIndexFile(file.absolutePath, null);
    }

    public IndexFile resolve(String path) {
        return files.get(path);
    }

    public List<IIndexFile> findIncludedFiles(IndexFile indexFile) {
        List<IIndexFile> includedFiles = new ArrayList<IIndexFile>();
        for (IIndexInclude include : indexFile.getIncludes()) {
            IndexFile includedFile = resolve(((IndexInclude)include).getPath());
            if (includedFile != null)
                includedFiles.add(includedFile);
        }
        return includedFiles;
    }

    public List<IndexFile> findIncludedBy(IndexFile indexFile) {
        List<IndexFile> result = new ArrayList<IndexFile>();
        for (Map.Entry<String,IndexFile> entry : files.entrySet()) {
            IndexFile file = entry.getValue();
            for (IIndexInclude include : file.includes) {
                String path = ((IndexInclude)include).getPath();
                if (path != null && path.equals(indexFile.absolutePath)) {
                    result.add(file);
                    break;
                }
            }
        }
        return result;
    }

    public void dumpToDebug() {
        Set<String> keys = new TreeSet<String>(files.keySet());
        for (String key : keys) {
            if (key.contains("networklayer/contract")) {
                IndexFile file = files.get(key);
                file.dumpToDebug();
            }
        }
    }

    class Indexer {
        Stack<IndexFile> activeIndexFiles = new Stack<IndexFile>();
        int withinOmnetpph = 0;

        /**
         * Called before the CPreprocessor starts scanning a toplevel source file.
         */
        public void handleTranslationUnitStart(String filename) {
            println("Start "+filename);
            Assert.isTrue(activeIndexFiles.isEmpty());
            Assert.isTrue(withinOmnetpph == 0);
            startIndexing(filename);
        }

        /**
         * Called when the CPreprocessor finished the scanning of a toplevel source file.
         */
        public void handleTranslationUnitEnd(String filename) {
            endIndexing(filename);
            println("End "+filename);
            Assert.isTrue(activeIndexFiles.isEmpty());
            Assert.isTrue(withinOmnetpph == 0);
        }

        /**
         * Called before the CPreprocessor starts scanning of an included file.
         */
        public void handleInclusionStart(IASTPreprocessorIncludeStatement include) {
            Assert.isTrue(include.isActive());
            println("start of "+include.getPath());
            for (IndexFile file : activeIndexFiles) {
                Assert.isTrue(!file.isComplete);
                file.addInclude(include, activeIndexFiles.peek());
            }
            startIndexing(include.getPath());
        }

        /**
         * Called when the CPreprocessor finished the scanning of an included file.
         */
        public void handleInclusionEnd(String filename) {
            endIndexing(filename);
            println("end of "+filename);
        }

        /**
         * Called when the CPreprocessor start processing of an include, that is
         * - skipped because it is a known system include
         * - skipped because it is already included into the current translation unit
         * - processed from the index, because it content has already been scanned
         */
        public void handleSkippedInclusion(IASTPreprocessorIncludeStatement include) {
            if (include.isActive()) {
                println("#include "+include.getPath());
                IndexFile includedFile = (IndexFile)resolveInclude(include);

                for (IndexFile file : activeIndexFiles) {
                    Assert.isTrue(!file.isComplete);
                    boolean added = file.addInclude(include, activeIndexFiles.peek());
                    if (added) {
                        if (includedFile != null && includedFile.isComplete) {
                            for (IIndexMacro macro : includedFile.getMacros())
                                if (macro instanceof IndexMacro)
                                    file.addDefine(((IndexMacro)macro).getMacroDef());
                                else if (macro instanceof IndexUndef)
                                    file.addUndef(((IndexUndef)macro).getUndefStatement());
                        }
                    }
                }
            }
        }

        /**
         * Called when the CPreprocessor encounters a macro definition.
         */
        public void handleDefine(IMacroBinding macrodef, boolean isActive) {
            if (isActive /*&& (withinOmnetpph == 0 || macrodef.getName().equals("OMNETPP_VERSION"))*/) {
                println("#define "+macrodef.getName()+(macrodef.isFunctionStyle()?"(...) " : " ")+new String(macrodef.getExpansion()));
                for (IndexFile file : activeIndexFiles) {
                    Assert.isTrue(!file.isComplete);
                    file.addDefine(macrodef);
                }
            }
        }

        /**
         * Called when the CPreprocessor encounters a #undef.
         */
        public void handleUndef(IASTPreprocessorUndefStatement undef) {
            if (undef.isActive() && withinOmnetpph == 0) {
                println("#undef "+undef.getMacroName().toString());
                for (IndexFile file : activeIndexFiles) {
                    Assert.isTrue(!file.isComplete);
                    file.addUndef(undef);
                }
            }
        }

        public void handleIfdef(IMacroBinding macro, boolean isActive) {
            if (macro != null) {
                addMacroReference(macro);
            }
        }

        public void handleIfndef(IMacroBinding macro, boolean isActive) {
            if (macro != null) {
                addMacroReference(macro);
            }
        }

        public void handleIf(IASTName[] refs, boolean isActive) {
            for (IASTName ref : refs) {
                if (ref.getBinding() instanceof IMacroBinding)
                    addMacroReference((IMacroBinding)ref.getBinding());
            }
        }

        public void handleElif(IASTName[] refs, boolean isActive) {
            for (IASTName ref : refs) {
                if (ref.getBinding() instanceof IMacroBinding)
                    addMacroReference((IMacroBinding)ref.getBinding());
            }
        }

        public void handleMacroExpansion(IMacroBinding outerMacro, IASTName[] implicitMacroReferences) {
            addMacroReference(outerMacro);
            for (IASTName ref : implicitMacroReferences) {
                if (ref.getBinding() instanceof IMacroBinding)
                    addMacroReference((IMacroBinding)ref.getBinding());
            }
        }

        protected IIndexFile resolveInclude(IASTPreprocessorIncludeStatement include) {
            println("Resolving "+include.getPath());
            if (include.isResolved()) {
                Assert.isTrue(!StringUtils.isEmpty(include.getPath()));
                return Index.this.resolve(include.getPath());
            }
            return null;
        }

        protected void startIndexing(String filename) {
            Assert.isTrue(!files.containsKey(filename));
            IndexFile indexFile = new IndexFile(filename);
            files.put(filename, indexFile);
            activeIndexFiles.push(indexFile);
            if (filename.endsWith("omnetpp.h"))
                withinOmnetpph++;
        }

        protected void endIndexing(String filename) {
            Assert.isTrue(!activeIndexFiles.isEmpty() && activeIndexFiles.peek().absolutePath.equals(filename));
            IndexFile file = activeIndexFiles.pop();
            file.freeze();
            fileContentProvider.setIndexFile(filename, file);
            if (filename.endsWith("omnetpp.h"))
                withinOmnetpph--;
        }

        protected void addMacroReference(IMacroBinding macro) {
            Assert.isTrue(!activeIndexFiles.isEmpty());
            activeIndexFiles.peek().addReferencedMacro(macro);
//            for (IndexFile file : activeIndexFiles) {
//                Assert.isTrue(!file.isComplete);
//                file.addReferencedMacro(macro);
//            }
        }

        private boolean debug = false;
        private void println(String msg) {
            if (debug) {
              Debug.print(StringUtils.leftPad("", 2*activeIndexFiles.size()));
              Debug.println(msg);
            }
        }
    }

    static class IndexFile implements IIndexFile {

        boolean isComplete;
        String absolutePath;
        List<IIndexInclude> includes = new ArrayList<IIndexInclude>();
        List<IIndexMacro> definedMacros = new ArrayList<IIndexMacro>();
        Map<String, String> referencedMacros;

        public IndexFile(String path) {
            this.absolutePath = path;
        }

        public void addDefine(IMacroBinding macrodef) {
            Assert.isTrue(!isComplete);
            definedMacros.add(new IndexMacro(macrodef));
        }

        public void addUndef(IASTPreprocessorUndefStatement undef) {
            Assert.isTrue(!isComplete);
            definedMacros.add(new IndexUndef(undef));
        }

        public boolean addInclude(IASTPreprocessorIncludeStatement include, IIndexFile includedBy) {
            Assert.isTrue(!isComplete);
            if (findInclude(include) == null) {
                includes.add(new IndexInclude(include, includedBy));
                return true;
            }
            else
                return false;
        }

        public void addReferencedMacro(IMacroBinding macrodef) {
            if (referencedMacros == null)
                referencedMacros = new HashMap<String,String>();
            String name = macrodef.getName();
            if (!referencedMacros.containsKey(name)) {
                String expansion = macrodef.getExpansion() != null ? new String(macrodef.getExpansion()) : null;
                referencedMacros.put(name, expansion);
            }
        }

        public void freeze() {
            isComplete = true;
        }

        public boolean isCompatibleWithMacroValues(IMacroValueProvider definedMacros) {
            if (referencedMacros == null)
                return true;

            for (Map.Entry<String,String> entry : referencedMacros.entrySet()) {
                if (!ObjectUtils.equals(entry.getValue(), definedMacros.getValue(entry.getKey())))
                    return false;
            }
            return true;
        }

        @Override
        public IIndexFileLocation getLocation() throws CoreException {
            return new IndexFileLocation(UNCPathConverter.getInstance().toURI(absolutePath), null);
        }

        @Override
        public IIndexInclude[] getIncludes() {
            Assert.isTrue(isComplete);
            return includes.toArray(new IIndexInclude[includes.size()]);
        }

        @Override
        public IIndexMacro[] getMacros() {
            Assert.isTrue(isComplete);
            return definedMacros.toArray(new IIndexMacro[definedMacros.size()]);
        }

        @Override
        public ICPPUsingDirective[] getUsingDirectives() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public long getTimestamp() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public long getContentsHash() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getScannerConfigurationHashcode() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getEncodingHashcode() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexName[] findNames(int offset, int length) throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexInclude getParsedInContext() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getLinkageID() throws CoreException {
            throw new UnsupportedOperationException();
        }

        public IIndexInclude findInclude(IASTPreprocessorIncludeStatement includeStatement) {
            for (IIndexInclude include : includes) {
                try {
                    if (include.getFullName().equals(includeStatement.getName().toString()))
                        return include;
                } catch (CoreException e) {}
            }
            return null;
        }

        public void dumpToDebug() {
            Debug.println("Index file " + absolutePath);
            if (referencedMacros != null) {
                Debug.print("  dependencies: ");
                StringBuilder sb = new StringBuilder();
                boolean first = true;
                for (Map.Entry<String,String> entry : referencedMacros.entrySet()) {
                    if (first)
                        first = false;
                    else
                        sb.append(",");
                    sb.append(entry.getKey());
                    if (entry.getValue() != null)
                        sb.append("(").append(entry.getValue()).append(")");
                }
                Debug.println(sb.toString());
            }
//            for (IIndexInclude include : includes)
//                ((IndexInclude)include).dumpToDebug();
//            for (IIndexMacro macro : definedMacros) {
//                if (macro instanceof IndexMacro)
//                    ((IndexMacro)macro).dumpToDebug();
//                else if (macro instanceof IndexUndef)
//                    ((IndexUndef)macro).dumpToDebug();
//            }
        }
    }

    static class IndexMacro implements IIndexMacro {
        IMacroBinding macrodef;

        public IndexMacro(IMacroBinding macrodef) {
            this.macrodef = macrodef;
        }

        public IMacroBinding getMacroDef() {
            return macrodef;
        }

        public void dumpToDebug() {
            Debug.println("#define "+getName()+(isFunctionStyle()?"(...) " : " ")+new String(getExpansion()));
        }

        @Override
        public boolean isFunctionStyle() {
            return macrodef.isFunctionStyle();
        }

        @Override
        public boolean isDynamic() {
            return macrodef.isDynamic();
        }

        @Override
        public char[][] getParameterList() {
            return macrodef.getParameterList();
        }

        @Override
        public char[] getExpansion() {
            return macrodef.getExpansion();
        }

        @Override
        public char[][] getParameterPlaceholderList() {
            return macrodef.getParameterPlaceholderList();
        }

        @Override
        public char[] getExpansionImage() {
            return macrodef.getExpansionImage();
        }

        @Override
        public String getName() {
            return macrodef.getName();
        }

        @Override
        public char[] getNameCharArray() {
            return macrodef.getNameCharArray();
        }

        @Override
        public ILinkage getLinkage() throws CoreException {
            return macrodef.getLinkage();
        }

        @Override
        public IIndexBinding getOwner() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IScope getScope() throws DOMException {
            return macrodef.getScope();
        }

        @Override
        public Object getAdapter(Class adapter) {
            return macrodef.getAdapter(adapter);
        }

        @Override
        public String[] getQualifiedName() {
            throw new UnsupportedOperationException();
        }

        @Override
        public boolean isFileLocal() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexFile getLocalToFile() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IASTFileLocation getFileLocation() throws CoreException {
            // throw new UnsupportedOperationException();
            return null;
        }

        @Override
        public IIndexFile getFile() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexName getDefinition() throws CoreException {
            throw new UnsupportedOperationException();
        }
    }

    static class IndexUndef implements IIndexMacro {

        IASTPreprocessorUndefStatement undef;

        public IndexUndef(IASTPreprocessorUndefStatement undef) {
            this.undef = undef;
        }

        public IASTPreprocessorUndefStatement getUndefStatement() {
            return undef;
        }

        public void dumpToDebug() {
            Debug.println("#undef "+getName());
        }

        @Override
        public boolean isFunctionStyle() {
            return false;
        }

        @Override
        public boolean isDynamic() {
            return false;
        }

        @Override
        public char[][] getParameterList() {
            return null;
        }

        @Override
        public char[] getExpansion() {
            return null;
        }

        @Override
        public char[][] getParameterPlaceholderList() {
            return null;
        }

        @Override
        public char[] getExpansionImage() {
            return null;
        }

        @Override
        public String getName() {
            return undef.getMacroName().toString();
        }

        @Override
        public char[] getNameCharArray() {
            return undef.getMacroName().toCharArray();
        }

        @Override
        public ILinkage getLinkage() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexBinding getOwner() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IScope getScope() throws DOMException {
            throw new UnsupportedOperationException();
        }

        @Override
        public Object getAdapter(Class adapter) {
            throw new UnsupportedOperationException();
        }

        @Override
        public String[] getQualifiedName() {
            throw new UnsupportedOperationException();
        }

        @Override
        public boolean isFileLocal() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexFile getLocalToFile() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IASTFileLocation getFileLocation() throws CoreException {
            //throw new UnsupportedOperationException();
            return null;
        }

        @Override
        public IIndexFile getFile() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexName getDefinition() throws CoreException {
            throw new UnsupportedOperationException();
        }
    }

    static class IndexInclude implements IIndexInclude {

        IASTPreprocessorIncludeStatement include;
        IIndexFile includedBy;

        public IndexInclude(IASTPreprocessorIncludeStatement include, IIndexFile includedBy) {
            this.include = include;
            this.includedBy = includedBy;
        }

        public void dumpToDebug() {
            try {
            Debug.println("#include "+(isSystemInclude()?"<" : "\"")+getName()+(isSystemInclude()?">" : "\""));
            } catch (CoreException e) {
                throw new RuntimeException(e);
            }
        }

        @Override
        public IIndexFile getIncludedBy() throws CoreException {
            return includedBy;
        }

        @Override
        public IIndexFileLocation getIncludedByLocation() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexFileLocation getIncludesLocation() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public String getName() throws CoreException {
            return include.getName().toString(); // XXX
        }

        @Override
        public String getFullName() throws CoreException {
            return include.getName().toString();
        }

        @Override
        public int getNameOffset() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getNameLength() throws CoreException {
            throw new UnsupportedOperationException();
        }

        @Override
        public boolean isSystemInclude() throws CoreException {
            return include.isSystemInclude();
        }

        @Override
        public boolean isActive() throws CoreException {
            return include.isActive();
        }

        @Override
        public boolean isResolved() throws CoreException {
            return include.isResolved();
        }

        @Override
        public boolean isResolvedByHeuristics() throws CoreException {
            return include.isResolvedByHeuristics();
        }

        public String getPath() {
            return include.isResolved() ? include.getPath() : null;
        }
    }
}
