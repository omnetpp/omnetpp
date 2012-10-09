package freemarker.eclipse.outline;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

import freemarker.eclipse.FreemarkerPlugin;
import freemarker.eclipse.ImageManager;
import freemarker.eclipse.preferences.IPreferenceConstants;

/**
 * @version $Id: OutlineLabelProvider.java,v 1.3 2003/08/15 19:49:08 stephanmueller Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 *
 * This class provides image and text information for elements of a template tree.
 */
public class OutlineLabelProvider
    extends LabelProvider
    implements IPreferenceConstants {

    private Image fMacroImage = null;
    private Image fFunctionImage = null;
    private Image fImportImage = null;
    private Image fImportCollectionImage = null;

    public OutlineLabelProvider() {
        super();
        fMacroImage = ImageManager.getImage(ImageManager.IMG_MACRO);
        fFunctionImage = ImageManager.getImage(ImageManager.IMG_FUNCTION);
        fImportImage = ImageManager.getImage(ImageManager.IMG_IMPORT);
        fImportCollectionImage = ImageManager.getImage(ImageManager.IMG_IMPORT_COLLECTION);
    }

    public Image getImage(Object anElement) {
        if (null == anElement)
            return null;
        boolean bShowIcons =
            FreemarkerPlugin.getInstance().getPreferenceStore().getBoolean(
                SHOW_ICONS);
        if (bShowIcons) {
            if (anElement instanceof ImportNode) {
                return fImportImage;
            } else if (anElement instanceof ImportCollectionNode) {
                return fImportCollectionImage;
            } else if (anElement instanceof MacroNode) {
                if(((MacroNode)anElement).isFunction()) {
                    return fFunctionImage;
                } else {
                    return fMacroImage;
                }
            } else {
                return null;
            }
        } else {
            return null;
        }
    }

    public String getText(Object anElement) {
        return anElement!=null?anElement.toString():"";
    }

    public void dispose() {
        if(fImportCollectionImage!=null) {fImportCollectionImage.dispose();}
        if(fImportImage!=null) {fImportImage.dispose();}
        if(fMacroImage!=null) {fMacroImage.dispose();}
        if(fFunctionImage!=null) {fFunctionImage.dispose();}
    }
}
