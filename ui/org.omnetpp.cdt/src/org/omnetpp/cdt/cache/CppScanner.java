package org.omnetpp.cdt.cache;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.List;
import java.util.Stack;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.dom.ast.IASTFileLocation;
import org.eclipse.cdt.core.dom.ast.IASTName;
import org.eclipse.cdt.core.dom.ast.IASTNode;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIncludeStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorMacroDefinition;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorUndefStatement;
import org.eclipse.cdt.core.dom.ast.IMacroBinding;
import org.eclipse.cdt.core.dom.parser.IScannerExtensionConfiguration;
import org.eclipse.cdt.core.dom.parser.cpp.GPPScannerExtensionConfiguration;
import org.eclipse.cdt.core.parser.EndOfFileException;
import org.eclipse.cdt.core.parser.FileContent;
import org.eclipse.cdt.core.parser.IParserLogService;
import org.eclipse.cdt.core.parser.IScannerInfo;
import org.eclipse.cdt.core.parser.IScannerInfoProvider;
import org.eclipse.cdt.core.parser.IToken;
import org.eclipse.cdt.core.parser.IncludeFileContentProvider;
import org.eclipse.cdt.core.parser.ParserFactory;
import org.eclipse.cdt.core.parser.ParserLanguage;
import org.eclipse.cdt.core.parser.ScannerInfo;
import org.eclipse.cdt.internal.core.dom.parser.ASTNode;
import org.eclipse.cdt.internal.core.parser.scanner.AbstractCharArray;
import org.eclipse.cdt.internal.core.parser.scanner.CPreprocessor;
import org.eclipse.cdt.internal.core.parser.scanner.ILocationCtx;
import org.eclipse.cdt.internal.core.parser.scanner.ILocationResolver;
import org.eclipse.cdt.internal.core.parser.scanner.ImageLocationInfo;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent;
import org.eclipse.cdt.internal.core.parser.scanner.LocationMap;
import org.eclipse.cdt.internal.core.parser.scanner.MacroExpander;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.cache.Index.Indexer;
import org.omnetpp.common.util.ReflectionUtils;

@SuppressWarnings("restriction")
class CppScanner {
    Scanner scanner;

    private static final IASTPreprocessorIncludeStatement[] EMPTY_INCLUDES = new IASTPreprocessorIncludeStatement[0];
    private static final IASTPreprocessorMacroDefinition[] EMPTY_MACROS = new IASTPreprocessorMacroDefinition[0];

    CppScanner(IFile file) {
        this.scanner = createScanner(file);
    }

    void scanFully() {
        if (scanner != null)
            scanner.scanFully();
    }

    IASTPreprocessorIncludeStatement[] getIncludeDirectives() {
        if (scanner == null)
            return EMPTY_INCLUDES;
        ILocationResolver locationResolver = scanner.getLocationResolver();
        return locationResolver.getIncludeDirectives();
    }

    IASTFileLocation getIncludeLocation(IASTPreprocessorIncludeStatement include) {
        if (include instanceof ASTNode && scanner != null) {
            ILocationResolver locationResolver = scanner.getLocationResolver();
            int offset = ((ASTNode)include).getOffset();
            int len = ((ASTNode)include).getLength();
            return locationResolver.getMappedFileLocation(offset, len);
        }
        return null;
    }

    IASTPreprocessorMacroDefinition[] getMacroDefinitions() {
        if (scanner == null)
            return EMPTY_MACROS;
        ILocationResolver locationResolver = scanner.getLocationResolver();
        return locationResolver.getMacroDefinitions();
    }

    private Scanner createScanner(IFile file)
    {
        IScannerInfo scanInfo = getScannerInfo(file);

        CachedFileContentProvider fileContentProvider = Activator.getFileContentProvider();
        FileContent fileContent = fileContentProvider.getContentForTranslationUnit(file.getLocation().toOSString());
        if (fileContent == null)
            return null;


        Scanner scanner= new Scanner(fileContent,
                                            scanInfo,
                                            ParserLanguage.CPP,
                                            ParserFactory.createDefaultLogService(),
                                            GPPScannerExtensionConfiguration.getInstance(),
                                            fileContentProvider);

        // replace fLocationMap field of the CPreprocessor with out customized version
        LocationMap oldLocationMap = (LocationMap)scanner.getLocationResolver();
        LocationMap newLocationMap = new LocationMap2(oldLocationMap);
        MacroExpander macroExpander = (MacroExpander)ReflectionUtils.getFieldValue(scanner, "fMacroExpander");
        ReflectionUtils.setFieldValue(scanner, "fLocationMap", newLocationMap);
        ReflectionUtils.setFieldValue(macroExpander, "fLocationMap", newLocationMap);

        return scanner;
    }

    private IScannerInfo getScannerInfo(IFile file) {
        IScannerInfoProvider provider = CCorePlugin.getDefault().getScannerInfoProvider(file.getProject());
        if (provider != null) {
           IScannerInfo buildScanInfo = provider.getScannerInformation(file);
           if (buildScanInfo != null) {
               return buildScanInfo;
           }
        }

        return new ScannerInfo(null, null);
    }

    /**
     * Extends CPreprocessor with notifications about the start/end of processing a translation unit.
     */
    private static class Scanner extends CPreprocessor {
        String filePath;

        public Scanner(FileContent fileContent, IScannerInfo info, ParserLanguage language, IParserLogService log,
                        IScannerExtensionConfiguration configuration, IncludeFileContentProvider readerFactory) {
            super(fileContent, info, language, log, configuration, readerFactory);
            this.filePath = ((InternalFileContent)fileContent).getFileLocation();
        }

        public void scanFully() {
            Indexer indexer = Activator.getIndex().getIndexer();
            indexer.handleTranslationUnitStart(filePath);
            try {
                while (nextToken().getType() != IToken.tEND_OF_INPUT) ;
            } catch (EndOfFileException e) {}
            indexer.handleTranslationUnitEnd(filePath);
        }
    }

    /**
     * Hacked LocationMap that notifies the Indexer about CPreprocessor events.
     */
    class LocationMap2 extends LocationMap {

        Stack<ILocationCtx> contexts = new Stack<ILocationCtx>();
        Indexer indexer;
        Field fDirectivesField;

        public LocationMap2(LocationMap other) {
            super(other.getLexerOptions());
            // copy fields from other
            Class<? super LocationMap> clazz = LocationMap.class;
            Assert.isTrue(other.getClass() == clazz);
            while (clazz != null) {
                for (Field field : clazz.getDeclaredFields()) {
                    int modifiers = field.getModifiers();
                    if (!Modifier.isStatic(modifiers) && !Modifier.isFinal(modifiers)) {
                        ReflectionUtils.setAccessible(field);
                        ReflectionUtils.setFieldValue(field, this, ReflectionUtils.getFieldValue(field, other));
                    }
                }
                clazz = clazz.getSuperclass();
            }
            // initialize our fields
            this.indexer = Activator.getIndex().getIndexer();
            this.fDirectivesField = ReflectionUtils.getField(getClass(), "fDirectives", false);
            ReflectionUtils.setAccessible(this.fDirectivesField);
        }

        /**
         * Returns the last processed preprocessor directive.
         * The directive must have the specified type.
         */
        @SuppressWarnings("unchecked")
        private <T> T getLastDirective(Class<T> clazz) {
            List<IASTNode> fDirectives = (List<IASTNode>)ReflectionUtils.getFieldValue(fDirectivesField, this);
            Assert.isTrue(!fDirectives.isEmpty());
            IASTNode lastDirective = fDirectives.get(fDirectives.size()-1);
            Assert.isTrue(clazz.isInstance(lastDirective));
            return (T)lastDirective;
        }

        /**
         * Called before the CPreprocessor starts scanning of an included file.
         */
        @Override
        public ILocationCtx pushInclusion(int startOffset, int nameOffset, int nameEndOffset, int endOffset, AbstractCharArray buffer,
                                          String filename, char[] name, boolean userInclude, boolean heuristic, boolean isSource) {
            ILocationCtx ctx = super.pushInclusion(startOffset, nameOffset, nameEndOffset, endOffset, buffer, filename, name, userInclude, heuristic, isSource);
            contexts.push(ctx);
            indexer.handleInclusionStart(getLastDirective(IASTPreprocessorIncludeStatement.class));
            return ctx;
        }


        @Override
        public ILocationCtx pushMacroExpansion(int nameOffset, int nameEndOffset, int endOffset, int contextLength, IMacroBinding macro,
                                                IASTName[] implicitMacroReferences, ImageLocationInfo[] imageLocations) {
            ILocationCtx ctx = super.pushMacroExpansion(nameOffset, nameEndOffset, endOffset, contextLength, macro, implicitMacroReferences, imageLocations);
            indexer.handleMacroExpansion(macro, implicitMacroReferences);
            return ctx;
        }

        /**
         * Called when the preprocessor finished the scanning of
         * - an included file
         * - the current translation unit
         * - a macro expansion
         */
        @Override
        public void popContext(ILocationCtx ctx) {
            if (!contexts.isEmpty() && contexts.peek() == ctx) {
                contexts.pop();
                indexer.handleInclusionEnd(ctx.getFilePath());
            }
            super.popContext(ctx);
        }

        /**
         * Called when the CPreprocessor encounters an #include directive, that is
         * - skipped because it is a known system include
         * - skipped because it is already included into the current translation unit
         * - processed from the index, because it content has already been scanned
         */
        @Override
        public void encounterPoundInclude(int startOffset, int nameOffset, int nameEndOffset, int endOffset, char[] name,
                                          String filename, boolean userInclude, boolean active, boolean heuristic) {
            super.encounterPoundInclude(startOffset, nameOffset, nameEndOffset, endOffset, name, filename, userInclude, active, heuristic);
            indexer.handleSkippedInclusion(getLastDirective(IASTPreprocessorIncludeStatement.class));
        }

        /**
         * Called when the CPreprocessor encounters a macro definition.
         */
        @Override
        public void encounterPoundDefine(int startOffset, int nameOffset, int nameEndOffset, int expansionOffset, int endOffset,
                                         boolean isActive, IMacroBinding macrodef) {
            super.encounterPoundDefine(startOffset, nameOffset, nameEndOffset, expansionOffset, endOffset, isActive, macrodef);
            indexer.handleDefine(macrodef, isActive);
        }

        /**
         * Called when the CPreprocessor encounters an #undef directive.
         */
        @Override
        public void encounterPoundUndef(IMacroBinding definition, int startOffset, int nameOffset, int nameEndOffset,
                                        int endOffset, char[] name, boolean isActive) {
            super.encounterPoundUndef(definition, startOffset, nameOffset, nameEndOffset, endOffset, name, isActive);
            indexer.handleUndef(getLastDirective(IASTPreprocessorUndefStatement.class));
        }

        @Override
        public void encounterPoundIfdef(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean taken, IMacroBinding macro) {
            super.encounterPoundIfdef(startOffset, condOffset, condEndOffset, endOffset, taken, macro);
            indexer.handleIfdef(macro, taken);
        }

        @Override
        public void encounterPoundIfndef(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean taken, IMacroBinding macro) {
            super.encounterPoundIfndef(startOffset, condOffset, condEndOffset, endOffset, taken, macro);
            indexer.handleIfndef(macro, taken);
        }

        @Override
        public void encounterPoundIf(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean isActive, IASTName[] macrosInDefinedExpression) {
            super.encounterPoundIf(startOffset, condOffset, condEndOffset, endOffset, isActive, macrosInDefinedExpression);
            indexer.handleIf(macrosInDefinedExpression, isActive);
        }

        @Override
        public void encounterPoundElif(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean isActive, IASTName[] macrosInDefinedExpression) {
            super.encounterPoundElif(startOffset, condOffset, condEndOffset, endOffset, isActive, macrosInDefinedExpression);
            indexer.handleElif(macrosInDefinedExpression, isActive);
        }
    }
}
