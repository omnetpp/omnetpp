package org.omnetpp.cdt.cache;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
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
        void encounterIf(IASTPreprocessorIfStatement if_);
        /**
         * Called after the scanner executed a #elif directive.
         */
        void encounterElif(IASTPreprocessorElifStatement elif);
        /**
         * Called on each reference of macros when expanding the condition of #if/#elif directives.
         */
        void encounterMacroReference(IASTName macroReference);
        /**
         * Called after the scanner performed a macro expansion.
         */
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
        Stack<ILocationCtx> includeContexts = new Stack<ILocationCtx>();
        IScannerEventListener listener;
        boolean isFollowedInclusion;

        private class NotifyingDirectivesList extends ArrayList<Object> {
            private static final long serialVersionUID = 1L;

            @Override
            public boolean add(Object e) {
                boolean result = super.add(e);
                if (listener != null) {
                    if (e instanceof IASTPreprocessorIncludeStatement) {
                        if (!isFollowedInclusion)
                            listener.encounterInclude((IASTPreprocessorIncludeStatement)e);
                    }
                    else if (e instanceof IASTPreprocessorMacroDefinition) {
                        listener.encounterDefine((IASTPreprocessorMacroDefinition)e);
                    }
                    else if (e instanceof IASTPreprocessorUndefStatement) {
                        listener.encounterUndef((IASTPreprocessorUndefStatement)e);
                    }
                    else if (e instanceof IASTPreprocessorIfdefStatement) {
                        listener.encounterIfdef((IASTPreprocessorIfdefStatement)e);
                    }
                    else if (e instanceof IASTPreprocessorIfndefStatement) {
                        listener.encounterIfndef((IASTPreprocessorIfndefStatement)e);
                    }
                    else if (e instanceof IASTPreprocessorIfStatement) {
                        listener.encounterIf((IASTPreprocessorIfStatement)e);
                    }
                    else if (e instanceof IASTPreprocessorElifStatement) {
                        listener.encounterElif((IASTPreprocessorElifStatement)e);
                    }
                }
                return result;
            }
        }

        private class NotifyingReferenceList extends ArrayList<Object> {
            private static final long serialVersionUID = 1L;

            @Override
            public boolean add(Object e) {
                boolean result = super.add(e);
                if (listener != null && e != null) {
                    IASTName name = (IASTName)e;
                    IASTNode parent = name.getParent();
                    if (parent instanceof IASTPreprocessorIfStatement) {
                        listener.encounterMacroReference(name);
                    }
                    else if (parent instanceof IASTPreprocessorElifStatement) {
                        listener.encounterMacroReference(name);
                    }
                }
                return result;
            }
        }

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
            // replace fDirectives and fMacroReferences
            ReflectionUtils.setFieldValue(this, "fDirectives", new NotifyingDirectivesList());
            ReflectionUtils.setFieldValue(this, "fMacroReferences", new NotifyingReferenceList());
            // initialize our fields
            this.listener = listener;
        }

        @Override
        public ILocationCtx pushInclusion(int startOffset, int nameOffset, int nameEndOffset, int endOffset, AbstractCharArray buffer,
                                          String filename, char[] name, boolean userInclude, boolean heuristic, boolean isSource) {
            try {
                isFollowedInclusion = true;
                ILocationCtx ctx = super.pushInclusion(startOffset, nameOffset, nameEndOffset, endOffset, buffer, filename, name, userInclude, heuristic, isSource);
                includeContexts.push(ctx);
                if (listener != null)
                    listener.startInclusion(ctx.getInclusionStatement());
                return ctx;
            }
            finally {
                isFollowedInclusion = false;
            }
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
                ILocationCtx context = includeContexts.peek();
                if (context == ctx) {
                    includeContexts.pop();
                    if (listener != null)
                        listener.endInclusion(context.getInclusionStatement());
                }
            }
            super.popContext(ctx);
        }
    }
}
