package freemarker.eclipse;

import java.net.MalformedURLException;
import java.net.URL;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;

/**
 * @version $Id:$
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class ImageManager {

    public static final String IMG_MACRO = "icons/userdefined_directive_call.gif";
    public static final String IMG_IMPORT = "icons/import.gif";
    public static final String IMG_IMPORT_COLLECTION = "icons/import_collection.gif";
    public static final String IMG_FUNCTION = "icons/function.gif";


    public static Image getImage(String filename) {
        ImageDescriptor temp = getImageDescriptor(filename);
        if(null!=temp) {
            return temp.createImage();
        } else {
            return null;
        }
    }

    public static ImageDescriptor getImageDescriptor(String filename) {
        try {
        URL url = new URL(FreemarkerPlugin.getInstance().getBundle().getEntry("/"),
                  filename);
                  return ImageDescriptor.createFromURL(url);
        } catch (MalformedURLException mue) {

        }
        return null;
    }

}
