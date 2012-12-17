package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;
import java.util.TreeSet;

import org.apache.commons.lang.StringUtils;
import org.eclipse.cdt.core.dom.ILinkage;
import org.eclipse.cdt.core.dom.ast.DOMException;
import org.eclipse.cdt.core.dom.ast.IASTFileLocation;
import org.eclipse.cdt.core.dom.ast.IASTName;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorElifStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorFunctionStyleMacroDefinition;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIfStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIfdefStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIfndefStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIncludeStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorMacroDefinition;
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
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
import org.eclipse.cdt.internal.core.dom.parser.ASTNode;
import org.eclipse.cdt.internal.core.index.IndexFileLocation;
import org.eclipse.cdt.utils.UNCPathConverter;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.cache.CppScanner.IScannerEventListener;
import org.omnetpp.cdt.cache.FileCache.ISourceFileChangeListener;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.Pair;

/**
 * This class implements some of the structures of the CDT index,
 * that can be fed into the CPreprocessor as input. The content
 * of the index is automatically invalidated when the source files
 * or the project descriptions change.
 * <p>
 * It also contains a builder, that generates these structures from
 * preprocessor events.
 *
 * @author tomi
 */
@SuppressWarnings("restriction")
public class Index {

    // maps file paths to index files
    private Map<String,IndexFile> files = new HashMap<String,IndexFile>();
    // builds index files from preprocessor events
    private Indexer indexer = new Indexer();

    private ISourceFileChangeListener sourceFileChangeListener = new ISourceFileChangeListener() {
        public void sourceFileChanged(IResourceDelta delta) {
            Index.this.sourceFileChanged(delta, false);
        }
    };

    private ICProjectDescriptionListener projectDescriptionListener = new ICProjectDescriptionListener() {
        public void handleEvent(CProjectDescriptionEvent e) {
            projectDescriptionChanged(e);
        }
    };

    public void hookListeners() {
        Activator.getFileCache().addFileChangeListener(sourceFileChangeListener);
        CoreModel.getDefault().addCProjectDescriptionListener(projectDescriptionListener, CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        Activator.getFileCache().removeFileChangeListener(sourceFileChangeListener);
        CoreModel.getDefault().removeCProjectDescriptionListener(projectDescriptionListener);
    }

    public int getNumberOfFiles() {
        return files.size();
    }

    public Indexer getIndexer() {
        return indexer;
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

    public List<IndexInclude> findIncludedBy(IndexFile file) {
        List<IndexInclude> result= new ArrayList<IndexInclude>();
        findIncludedBy(Collections.singletonList(file), result, new HashSet<IIndexFileLocation>());
        return result;
    }

    private void findIncludedBy(List<IndexFile> in, List<IndexInclude> out, Set<IIndexFileLocation> handled) {
        List<IndexFile> currentFiles = in;
        while (!currentFiles.isEmpty()) {
            List<IndexFile> nextLevel= new LinkedList<IndexFile>();
            for (IndexFile file : currentFiles) {
                for (IndexFile indexFile : files.values()) {
                    for (IndexInclude include : indexFile.findIncludes(file)) {
                        if (handled.add(include.getIncludedByLocation())) {
                            out.add(include);
                            nextLevel.add((IndexFile)include.getIncludedBy());
                        }
                    }
                }
            }
            currentFiles = nextLevel;
        }
    }

    private void sourceFileChanged(IResourceDelta delta, boolean purgeAll) {
        if (purgeAll) {
            Debug.format("Index: source file changed, discarding index\n");
            files.clear();
        }
        else {
            IndexFile indexFile = resolve(delta.getResource().getLocation().toOSString());
            if (indexFile != null) {
                Debug.format("Index: source file %s changed, discarding index\n", indexFile.absolutePath);
                removeFromIndex(indexFile);
                int count = 1;
                for (IndexInclude include : findIncludedBy(indexFile)) {
                    removeFromIndex((IndexFile)include.getIncludedBy());
                    count++;
                }
                Debug.format("  removed %d/%d files\n", count, count+files.size());
            }
        }
    }

    private void projectDescriptionChanged(CProjectDescriptionEvent e) {
        Debug.println("Index: project description changed, discarding index");
        files.clear();
    }

    private void removeFromIndex(IndexFile file) {
        files.remove(file.absolutePath);
    }

    public void dumpToDebug() {
        Debug.format("Index contains %d files\n", files.size());
        Set<String> keys = new TreeSet<String>(files.keySet());
        for (String key : keys) {
            IndexFile file = files.get(key);
            file.dumpToDebug();
        }
    }

    /**
     * TODO
     */
    class Indexer implements IScannerEventListener {
        private IndexFile currentIndexFile;
        private Stack<IndexFile> activeIndexFiles = new Stack<IndexFile>();
        private int withinOmnetpph = 0;

        public void startTranslationUnit(String filename) {
            println("Start "+filename);
            Assert.isTrue(activeIndexFiles.isEmpty());
            Assert.isTrue(withinOmnetpph == 0);
            startIndexing(filename);
        }

        public void endTranslationUnit(String filename) {
            endIndexing(filename);
            println("End "+filename);
            Assert.isTrue(activeIndexFiles.isEmpty());
            Assert.isTrue(withinOmnetpph == 0);
        }

        public void startInclusion(IASTPreprocessorIncludeStatement include) {
            Assert.isTrue(include.isActive());
            println("start of "+include.getPath());
            currentIndexFile.addInclude(include);
            startIndexing(include.getPath());
        }

        public void endInclusion(IASTPreprocessorIncludeStatement include) {
            endIndexing(include.getPath());
            println("end of "+include.getPath());
        }

        public void encounterInclude(IASTPreprocessorIncludeStatement include) {
            if (include.isActive()) {
                println("#include "+include.getPath());
                currentIndexFile.addInclude(include);
            }
        }

        public void encounterDefine(IASTPreprocessorMacroDefinition macrodef) {
            if (macrodef.isActive() /*&& (withinOmnetpph == 0 || macrodef.getName().equals("OMNETPP_VERSION"))*/) {
                println("#define "+macrodef.getName()+(macrodef instanceof IASTPreprocessorFunctionStyleMacroDefinition?"(...) " : " ")+new String(macrodef.getExpansion()));
                currentIndexFile.addDefine(macrodef);
            }
        }

        public void encounterUndef(IASTPreprocessorUndefStatement undef) {
            if (undef.isActive() && withinOmnetpph == 0) {
                println("#undef "+undef.getMacroName().toString());
                currentIndexFile.addUndef(undef);
            }
        }

        public void encounterIfdef(IASTPreprocessorIfdefStatement ifdef) {
            IASTName macroRef = ifdef.getMacroReference();
            if (macroRef != null) {
                addReferencedMacro((IMacroBinding)macroRef.getBinding());
            }
        }

        public void encounterIfndef(IASTPreprocessorIfndefStatement ifndef) {
            IASTName macroRef = ifndef.getMacroReference();
            if (macroRef != null) {
                addReferencedMacro((IMacroBinding)macroRef.getBinding());
            }
        }

        public void encounterIf(IASTPreprocessorIfStatement if_, IASTName[] refs) {
            for (IASTName ref : refs) {
                if (ref.getBinding() instanceof IMacroBinding)
                    addReferencedMacro((IMacroBinding)ref.getBinding());
            }
        }

        public void encounterElif(IASTPreprocessorElifStatement elif, IASTName[] refs) {
            for (IASTName ref : refs) {
                if (ref.getBinding() instanceof IMacroBinding)
                    addReferencedMacro((IMacroBinding)ref.getBinding());
            }
        }

        public void encounterMacroExpansion(IMacroBinding outerMacro, IASTName[] implicitMacroReferences) {
            addReferencedMacro(outerMacro);
            for (IASTName ref : implicitMacroReferences) {
                if (ref.getBinding() instanceof IMacroBinding)
                    addReferencedMacro((IMacroBinding)ref.getBinding());
            }
        }

        protected IIndexFile resolveInclude(IASTPreprocessorIncludeStatement include) {
            if (include.isResolved()) {
                Assert.isTrue(!StringUtils.isEmpty(include.getPath()));
                return Index.this.resolve(include.getPath());
            }
            return null;
        }

        protected void startIndexing(String filename) {
            //Assert.isTrue(!files.containsKey(filename));
            currentIndexFile = new IndexFile(filename);
            if (!files.containsKey(filename))
                files.put(filename, currentIndexFile);
            activeIndexFiles.push(currentIndexFile);
            if (filename.endsWith("omnetpp.h"))
                withinOmnetpph++;
        }

        protected void endIndexing(String filename) {
            Assert.isTrue(!activeIndexFiles.isEmpty() && activeIndexFiles.peek().absolutePath.equals(filename));
            IndexFile file = activeIndexFiles.pop();
            file.freeze();
            currentIndexFile = activeIndexFiles.isEmpty() ? null : activeIndexFiles.peek();
            if (filename.endsWith("omnetpp.h"))
                withinOmnetpph--;
        }

        protected void addReferencedMacro(IMacroBinding ref) {
            currentIndexFile.addReferencedMacro(ref);
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

        private final String absolutePath;
        private boolean isFrozen;
        private List<IndexInclude> includes = new ArrayList<IndexInclude>();
        private List<Object> preprocessingDirectives = new ArrayList<Object>(); // includes + defines + references
        private int countOfMacros;

        public IndexFile(String path) {
            this.absolutePath = path;
        }

        public void addDefine(IASTPreprocessorMacroDefinition macrodef) {
            Assert.isTrue(!isFrozen);
            preprocessingDirectives.add(new IndexMacro(macrodef));
            countOfMacros++;
        }

        public void addUndef(IASTPreprocessorUndefStatement undef) {
            Assert.isTrue(!isFrozen);
            preprocessingDirectives.add(new IndexUndef(undef));
            countOfMacros++;
        }

        public boolean addInclude(IASTPreprocessorIncludeStatement include) {
            Assert.isTrue(!isFrozen);
            if (findInclude(include) == null) {
                IndexInclude indexInclude = new IndexInclude(include, this);
                includes.add(indexInclude);
                preprocessingDirectives.add(indexInclude);
                return true;
            }
            else
                return false;
        }

        public void addReferencedMacro(IMacroBinding macrodef) {
            Assert.isTrue(!isFrozen);
            preprocessingDirectives.add(new Pair<String,char[]>(macrodef.getName(), macrodef.getExpansion()));
        }

        public void freeze() {
            isFrozen = true;
        }

        public String getPath() {
            return absolutePath;
        }

        public List<Object> getPreprocessingDirectives() {
            Assert.isTrue(isFrozen);
            return preprocessingDirectives;
        }

        @Override
        public IIndexFileLocation getLocation() {
            return new IndexFileLocation(UNCPathConverter.getInstance().toURI(absolutePath), null);
        }

        @Override
        public IIndexInclude[] getIncludes() {
            Assert.isTrue(isFrozen);
            return includes.toArray(new IIndexInclude[includes.size()]);
        }

        @Override
        public IIndexMacro[] getMacros() {
            Assert.isTrue(isFrozen);
            IIndexMacro[] result = new IIndexMacro[countOfMacros];
            int i = 0;
            for (Object directive : preprocessingDirectives)
                if (directive instanceof IIndexMacro)
                    result[i++] = (IIndexMacro)directive;
            return result;
        }

        @Override
        public ICPPUsingDirective[] getUsingDirectives() {
            throw new UnsupportedOperationException();
        }

        @Override
        public long getTimestamp() {
            throw new UnsupportedOperationException();
        }

        @Override
        public long getContentsHash() {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getScannerConfigurationHashcode() {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getEncodingHashcode() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexName[] findNames(int offset, int length) {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexInclude getParsedInContext() {
            throw new UnsupportedOperationException();
        }

        @Override
        public int getLinkageID() {
            throw new UnsupportedOperationException();
        }

        public IndexInclude findInclude(IASTPreprocessorIncludeStatement includeStatement) {
            for (IndexInclude include : includes) {
                if (include.getFullName().equals(includeStatement.getName().toString()))
                    return include;
            }
            return null;
        }

        public List<IndexInclude> findIncludes(IndexFile includedFile) {
            List<IndexInclude> result = new ArrayList<IndexInclude>();
            for (IndexInclude include : includes) {
                    String path = include.getPath();
                    if (path != null && path.equals(includedFile.absolutePath))
                        result.add(include);
            }
            return result;
        }

        public void dumpToDebug() {
            Debug.println("Index file " + absolutePath);
            if (preprocessingDirectives.size() > countOfMacros + includes.size()) {
                Debug.print("  dependencies: ");
                StringBuilder sb = new StringBuilder();
                boolean first = true;
                for (Object directive : preprocessingDirectives) {
                    if (directive instanceof IMacroBinding) {
                        IMacroBinding ref = (IMacroBinding)directive;
                        if (first)
                            first = false;
                        else
                            sb.append(",");
                        sb.append(ref.getName());
                        if (ref.getExpansion() != null)
                            sb.append("(").append(ref.getExpansion()).append(")");
                    }
                }
                Debug.println(sb.toString());
            }
            for (Object directive : preprocessingDirectives) {
                if (directive instanceof IndexInclude)
                    ((IndexInclude)directive).dumpToDebug();
                else if (directive instanceof IndexMacro)
                    ((IndexMacro)directive).dumpToDebug();
                else if (directive instanceof IndexUndef)
                    ((IndexUndef)directive).dumpToDebug();
            }
        }
    }

    static class IndexMacro implements IIndexMacro {
        private final IASTPreprocessorMacroDefinition define;
        private final IMacroBinding macrodef;

        public IndexMacro(IASTPreprocessorMacroDefinition define) {
            this.define = define;
            this.macrodef = (IMacroBinding)define.getName().getBinding();
        }

        public IASTPreprocessorMacroDefinition getDefineStatement() {
            return define;
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
        public Object getAdapter(@SuppressWarnings("rawtypes") Class adapter) {
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
        public IASTFileLocation getFileLocation() {
            return define.getFileLocation();
        }

        @Override
        public IIndexFile getFile() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexName getDefinition() {
            throw new UnsupportedOperationException();
        }
    }

    static class IndexUndef implements IIndexMacro {

        private final IASTPreprocessorUndefStatement undef;

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
        public Object getAdapter(@SuppressWarnings("rawtypes") Class adapter) {
            throw new UnsupportedOperationException();
        }

        @Override
        public String[] getQualifiedName() {
            throw new UnsupportedOperationException();
        }

        @Override
        public boolean isFileLocal() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexFile getLocalToFile() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IASTFileLocation getFileLocation() {
            return undef.getFileLocation();
        }

        @Override
        public IIndexFile getFile() {
            throw new UnsupportedOperationException();
        }

        @Override
        public IIndexName getDefinition() {
            throw new UnsupportedOperationException();
        }
    }

    static class IndexInclude implements IIndexInclude {

        private final IASTPreprocessorIncludeStatement include;
        private final IndexFile includedBy;

        public IndexInclude(IASTPreprocessorIncludeStatement include, IndexFile includedBy) {
            this.include = include;
            this.includedBy = includedBy;
        }

        public void dumpToDebug() {
            Debug.println("#include "+(isSystemInclude()?"<" : "\"")+getFullName()+(isSystemInclude()?">" : "\""));
        }

        @Override
        public IIndexFile getIncludedBy() {
            return includedBy;
        }

        @Override
        public IIndexFileLocation getIncludedByLocation() {
            return includedBy.getLocation();
        }

        @Override
        public IIndexFileLocation getIncludesLocation() {
            throw new UnsupportedOperationException();
        }

        @Override
        public String getName() {
            Path path = new Path(getFullName());
            return path.lastSegment();
        }

        @Override
        public String getFullName() {
            return include.getName().toString();
        }

        @Override
        public int getNameOffset() {
            return ((ASTNode)include.getName()).getOffset();
        }

        @Override
        public int getNameLength() {
            return ((ASTNode)include.getName()).getLength();
        }

        @Override
        public boolean isSystemInclude() {
            return include.isSystemInclude();
        }

        @Override
        public boolean isActive() {
            return include.isActive();
        }

        @Override
        public boolean isResolved() {
            return include.isResolved();
        }

        @Override
        public boolean isResolvedByHeuristics() {
            return include.isResolvedByHeuristics();
        }

        public String getPath() {
            return include.isResolved() ? include.getPath() : null;
        }
    }
}
