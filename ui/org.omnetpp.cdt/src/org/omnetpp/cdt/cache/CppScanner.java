package org.omnetpp.cdt.cache;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.List;
import java.util.Stack;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.dom.ast.IASTName;
import org.eclipse.cdt.core.dom.ast.IASTNode;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorElifStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIfStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIfdefStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIfndefStatement;
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
import org.eclipse.cdt.core.parser.util.CharArrayMap;
import org.eclipse.cdt.internal.core.parser.scanner.AbstractCharArray;
import org.eclipse.cdt.internal.core.parser.scanner.CPreprocessor;
import org.eclipse.cdt.internal.core.parser.scanner.ILocationCtx;
import org.eclipse.cdt.internal.core.parser.scanner.ImageLocationInfo;
import org.eclipse.cdt.internal.core.parser.scanner.InternalFileContent;
import org.eclipse.cdt.internal.core.parser.scanner.LocationMap;
import org.eclipse.cdt.internal.core.parser.scanner.MacroExpander;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.cdt.cache.FileContentProvider.IMacroValueProvider;
import org.omnetpp.common.util.ReflectionUtils;

/**
 * Scanner of C++ source and header files.
 * This class allows to register a listener for preprocessor directives.
 * <p>
 * Warning:
 * @author tomi
 */
@SuppressWarnings("restriction")
class CppScanner implements IMacroValueProvider {

    interface IScannerEventListener {
        /**
         * Called before scanning the translation unit.
         */
        void startTranslationUnit(String path);
        /**
         * Called after scanning the translation unit.
         */
        void endTranslationUnit(String path);
        /**
         * Called when a new file inclusion starts.
         */
        void startInclusion(IASTPreprocessorIncludeStatement include);
        /**
         * Called when the file inclusion finished.
         */
        void endInclusion(IASTPreprocessorIncludeStatement include);
        /**
         * Called when the scanner encounters an #include directive, that is
         * - skipped because the target can not be resolved
         * - skipped because it is a known system include
         * - skipped because it is already included into the current translation unit
         * - processed from the index, because its content has already been scanned
         */
        void encounterInclude(IASTPreprocessorIncludeStatement include);
        /**
         * Called after the scanner executed a #define directive.
         */
        void encounterDefine(IASTPreprocessorMacroDefinition define);
        /**
         * Called after the scanner executed a #undef directive.
         */
        void encounterUndef(IASTPreprocessorUndefStatement undef);
        /**
         * Called after the scanner executed a #ifdef directive.
         */
        void encounterIfdef(IASTPreprocessorIfdefStatement ifdef);
        /**
         * Called after the scanner executed a #ifndef directive.
         */
        void encounterIfndef(IASTPreprocessorIfndefStatement ifndef);
        /**
         * Called after the scanner executed a #if directive.
         */
        void encounterIf(IASTPreprocessorIfStatement if_, IASTName[] macroReferences);
        /**
         * Called after the scanner executed a #elif directive.
         */
        void encounterElif(IASTPreprocessorElifStatement elif, IASTName[] macroReferences);
        /**
         * Called after the scanner performed a macro expansion.
         */
        //void encounterMacroExpansion(IASTPreprocessorMacroExpansion expansion);
        void encounterMacroExpansion(IMacroBinding outerMacro, IASTName[] implicitMacroReferences);
    }

    Scanner scanner; // customized CPreprocessor

    CppScanner(FileContentProvider fileContentProvider, IScannerEventListener listener) {
        IFile translationUnit = fileContentProvider.getTranslationUnit();
        FileContent fileContent = fileContentProvider.getContentForTranslationUnit();
        if (fileContent != null) {
            this.scanner = new Scanner(fileContent,
                                       getScannerInfo(translationUnit),
                                       ParserLanguage.CPP,
                                       ParserFactory.createDefaultLogService(),
                                       GPPScannerExtensionConfiguration.getInstance(),
                                       fileContentProvider,
                                       listener);
        }
    }

    void scanFully() {
        if (scanner != null)
            scanner.scanFully();
    }

    private static IScannerInfo getScannerInfo(IFile file) {
        IScannerInfoProvider provider = CCorePlugin.getDefault().getScannerInfoProvider(file.getProject());
        if (provider != null) {
           IScannerInfo buildScanInfo = provider.getScannerInformation(file);
           if (buildScanInfo != null) {
               return buildScanInfo;
           }
        }

        return new ScannerInfo(null, null);
    }


    @Override
    public char[] getMacroValue(String name) {
        if (scanner == null)
            return null;
        CharArrayMap<IMacroBinding> macroDictionary = scanner.getDefinedMacros();
        IMacroBinding binding = macroDictionary.get(name.toCharArray());
        return binding != null ? binding.getExpansion() : null;
    }

    /**
     * Extends CPreprocessor with notifications about the start/end of processing a translation unit.
     */
    private static class Scanner extends CPreprocessor {
        String filePath;
        IScannerEventListener listener;
        Field fMacroDictionaryField;

        public Scanner(FileContent fileContent, IScannerInfo info, ParserLanguage language, IParserLogService log,
                        IScannerExtensionConfiguration configuration, IncludeFileContentProvider readerFactory,
                        IScannerEventListener listener) {
            super(fileContent, info, language, log, configuration, readerFactory);
            this.filePath = ((InternalFileContent)fileContent).getFileLocation();
            this.listener = listener;
            this.fMacroDictionaryField = ReflectionUtils.getField(getClass(), "fMacroDictionary", false);
            ReflectionUtils.setAccessible(this.fMacroDictionaryField);

            // replace fLocationMap field with our customized version
            LocationMap oldLocationMap = (LocationMap)getLocationResolver();
            LocationMap newLocationMap = new NotifyingLocationMap(oldLocationMap, listener);
            MacroExpander macroExpander = (MacroExpander)ReflectionUtils.getFieldValue(this, "fMacroExpander");
            ReflectionUtils.setFieldValue(this, "fLocationMap", newLocationMap);
            ReflectionUtils.setFieldValue(macroExpander, "fLocationMap", newLocationMap);
        }

        @SuppressWarnings("unchecked")
        public CharArrayMap<IMacroBinding> getDefinedMacros() {
            return (CharArrayMap<IMacroBinding>)ReflectionUtils.getFieldValue(fMacroDictionaryField, this);
        }

        public void scanFully() {
            if (listener != null)
                listener.startTranslationUnit(filePath);

            try {
                while (nextToken().getType() != IToken.tEND_OF_INPUT) ;
            } catch (EndOfFileException e) {}

            if (listener != null)
                listener.endTranslationUnit(filePath);
        }
    }

    private static class NotifyingLocationMap extends LocationMap {
        private static class IncludeContext {
            final ILocationCtx locationCtx;
            final IASTPreprocessorIncludeStatement include;

            public IncludeContext(ILocationCtx locationCtx, IASTPreprocessorIncludeStatement include) {
                this.locationCtx = locationCtx;
                this.include = include;
            }
        }

        Stack<IncludeContext> includeContexts = new Stack<IncludeContext>();
        IScannerEventListener listener;
        Field fDirectivesField;

        public NotifyingLocationMap(LocationMap other, IScannerEventListener listener) {
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
            this.listener = listener;
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

        @Override
        public ILocationCtx pushInclusion(int startOffset, int nameOffset, int nameEndOffset, int endOffset, AbstractCharArray buffer,
                                          String filename, char[] name, boolean userInclude, boolean heuristic, boolean isSource) {
            ILocationCtx ctx = super.pushInclusion(startOffset, nameOffset, nameEndOffset, endOffset, buffer, filename, name, userInclude, heuristic, isSource);
            IASTPreprocessorIncludeStatement include = getLastDirective(IASTPreprocessorIncludeStatement.class);
            includeContexts.push(new IncludeContext(ctx, include));
            if (listener != null)
                listener.startInclusion(include);
            return ctx;
        }


        @Override
        public ILocationCtx pushMacroExpansion(int nameOffset, int nameEndOffset, int endOffset, int contextLength, IMacroBinding macro,
                                                IASTName[] implicitMacroReferences, ImageLocationInfo[] imageLocations) {
            ILocationCtx ctx = super.pushMacroExpansion(nameOffset, nameEndOffset, endOffset, contextLength, macro, implicitMacroReferences, imageLocations);
            if (listener != null)
                listener.encounterMacroExpansion(macro, implicitMacroReferences);
            return ctx;
        }

        @Override
        public void popContext(ILocationCtx ctx) {
            if (!includeContexts.isEmpty()) {
                IncludeContext context = includeContexts.peek();
                if (context.locationCtx == ctx) {
                    includeContexts.pop();
                    if (listener != null)
                        listener.endInclusion(context.include);
                }
            }
            super.popContext(ctx);
        }

        @Override
        public void encounterPoundInclude(int startOffset, int nameOffset, int nameEndOffset, int endOffset, char[] name,
                                          String filename, boolean userInclude, boolean active, boolean heuristic) {
            super.encounterPoundInclude(startOffset, nameOffset, nameEndOffset, endOffset, name, filename, userInclude, active, heuristic);
            if (listener != null)
                listener.encounterInclude(getLastDirective(IASTPreprocessorIncludeStatement.class));
        }

        @Override
        public void encounterPoundDefine(int startOffset, int nameOffset, int nameEndOffset, int expansionOffset, int endOffset,
                                         boolean isActive, IMacroBinding macrodef) {
            super.encounterPoundDefine(startOffset, nameOffset, nameEndOffset, expansionOffset, endOffset, isActive, macrodef);
            if (listener != null)
                listener.encounterDefine(getLastDirective(IASTPreprocessorMacroDefinition.class));
        }

        @Override
        public void encounterPoundUndef(IMacroBinding definition, int startOffset, int nameOffset, int nameEndOffset,
                                        int endOffset, char[] name, boolean isActive) {
            super.encounterPoundUndef(definition, startOffset, nameOffset, nameEndOffset, endOffset, name, isActive);
            if (listener != null)
                listener.encounterUndef(getLastDirective(IASTPreprocessorUndefStatement.class));
        }

        @Override
        public void encounterPoundIfdef(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean taken, IMacroBinding macro) {
            super.encounterPoundIfdef(startOffset, condOffset, condEndOffset, endOffset, taken, macro);
            if (listener != null)
                listener.encounterIfdef(getLastDirective(IASTPreprocessorIfdefStatement.class));
        }

        @Override
        public void encounterPoundIfndef(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean taken, IMacroBinding macro) {
            super.encounterPoundIfndef(startOffset, condOffset, condEndOffset, endOffset, taken, macro);
            if (listener != null)
                listener.encounterIfndef(getLastDirective(IASTPreprocessorIfndefStatement.class));
        }

        @Override
        public void encounterPoundIf(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean isActive, IASTName[] macrosInDefinedExpression) {
            super.encounterPoundIf(startOffset, condOffset, condEndOffset, endOffset, isActive, macrosInDefinedExpression);
            if (listener != null)
                listener.encounterIf(getLastDirective(IASTPreprocessorIfStatement.class), macrosInDefinedExpression);
        }

        @Override
        public void encounterPoundElif(int startOffset, int condOffset, int condEndOffset, int endOffset, boolean isActive, IASTName[] macrosInDefinedExpression) {
            super.encounterPoundElif(startOffset, condOffset, condEndOffset, endOffset, isActive, macrosInDefinedExpression);
            if (listener != null)
                listener.encounterElif(getLastDirective(IASTPreprocessorElifStatement.class), macrosInDefinedExpression);
        }
    }
}
