package org.omnetpp.cdt.cache;

import org.eclipse.cdt.core.CCorePlugin;
import org.eclipse.cdt.core.dom.ast.IASTFileLocation;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorIncludeStatement;
import org.eclipse.cdt.core.dom.ast.IASTPreprocessorMacroDefinition;
import org.eclipse.cdt.core.dom.parser.cpp.GPPScannerExtensionConfiguration;
import org.eclipse.cdt.core.parser.EndOfFileException;
import org.eclipse.cdt.core.parser.FileContent;
import org.eclipse.cdt.core.parser.IScanner;
import org.eclipse.cdt.core.parser.IScannerInfo;
import org.eclipse.cdt.core.parser.IScannerInfoProvider;
import org.eclipse.cdt.core.parser.IToken;
import org.eclipse.cdt.core.parser.ParserFactory;
import org.eclipse.cdt.core.parser.ParserLanguage;
import org.eclipse.cdt.core.parser.ScannerInfo;
import org.eclipse.cdt.internal.core.dom.parser.ASTNode;
import org.eclipse.cdt.internal.core.parser.scanner.CPreprocessor;
import org.eclipse.cdt.internal.core.parser.scanner.ILocationResolver;
import org.eclipse.core.resources.IFile;
import org.omnetpp.cdt.Activator;

@SuppressWarnings("restriction")
class CppScanner {
    IScanner scanner;

    private static final IASTPreprocessorIncludeStatement[] EMPTY_INCLUDES = new IASTPreprocessorIncludeStatement[0];
    private static final IASTPreprocessorMacroDefinition[] EMPTY_MACROS = new IASTPreprocessorMacroDefinition[0];

    CppScanner(IFile file) {
        this.scanner = createScanner(file);
    }

    void scanFully() {
        if (scanner != null) {
            try {
                while (scanner.nextToken().getType() != IToken.tEND_OF_INPUT) ;
            } catch (EndOfFileException e) {}
        }
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

    private IScanner createScanner(IFile file)
    {
        IScannerInfo scanInfo = getScannerInfo(file);

        CachedFileContentProvider fileContentProvider = Activator.getFileContentProvider();
        FileContent fileContent = fileContentProvider.getContentForTranslationUnit(file.getLocation().toOSString());
        if (fileContent == null)
            return null;

        IScanner scanner= new CPreprocessor(fileContent,
                                            scanInfo,
                                            ParserLanguage.CPP,
                                            ParserFactory.createDefaultLogService(),
                                            GPPScannerExtensionConfiguration.getInstance(),
                                            fileContentProvider);

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
}
