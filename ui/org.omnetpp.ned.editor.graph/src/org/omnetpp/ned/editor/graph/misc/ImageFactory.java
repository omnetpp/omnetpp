package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.core.runtime.Path;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;

public class ImageFactory {
    private final static String IMAGE_DIR = "/images/"; 
    private final static String INTERNAL_DIR = "_internal/"; 
    private final static String TOOL_IMAGE_DIR = INTERNAL_DIR + "toolbar/";
    private final static String MODEL_IMAGE_DIR = INTERNAL_DIR + "model/";
    
    public final static String TOOLBAR_IMAGE_WIZARDBANNER = TOOL_IMAGE_DIR + "module";
    public final static String TOOLBAR_IMAGE_GROW = TOOL_IMAGE_DIR + "grow";
    public final static String TOOLBAR_IMAGE_SHRINK = TOOL_IMAGE_DIR + "shrink";
    public final static String TOOLBAR_IMAGE_OVERVIEW = TOOL_IMAGE_DIR + "overview";
    public final static String TOOLBAR_IMAGE_OUTLINE = TOOL_IMAGE_DIR + "outline";
    public final static String TOOLBAR_IMAGE_ZOOMMINUS = TOOL_IMAGE_DIR + "zoomminus";
    public final static String TOOLBAR_IMAGE_ZOOMPLUS = TOOL_IMAGE_DIR + "zoomplus";
    public final static String TOOLBAR_IMAGE_TEMPLATE = TOOL_IMAGE_DIR + "template";
    public final static String TOOLBAR_IMAGE_CONNECTION = TOOL_IMAGE_DIR + "connection";
    public final static String TOOLBAR_IMAGE_MODULE = TOOL_IMAGE_DIR + "module";
    public final static String TOOLBAR_IMAGE_SIMPLE = TOOL_IMAGE_DIR + "simple";
    public final static String TOOLBAR_IMAGE_LABEL = TOOL_IMAGE_DIR + "label";
    
    public final static String MODEL_IMAGE_CHANNEL = MODEL_IMAGE_DIR + "Channel";
    public final static String MODEL_IMAGE_COMPOUNDMODULE = MODEL_IMAGE_DIR + "CompoundModule";
    public final static String MODEL_IMAGE_CONNECTION = MODEL_IMAGE_DIR + "Connection";
    public final static String MODEL_IMAGE_GATE = MODEL_IMAGE_DIR + "Gate";
    public final static String MODEL_IMAGE_IMPORT = MODEL_IMAGE_DIR + "Import";
    public final static String MODEL_IMAGE_INVALID = MODEL_IMAGE_DIR + "Invalid";
    public final static String MODEL_IMAGE_PARAM = MODEL_IMAGE_DIR + "Param";
    public final static String MODEL_IMAGE_PROPERTY = MODEL_IMAGE_DIR + "Property";
    public final static String MODEL_IMAGE_SIMPLEMODULE = MODEL_IMAGE_DIR + "SimpleModule";
    public final static String MODEL_IMAGE_SUBMODULE = MODEL_IMAGE_DIR + "Submodule";

    private final static String DEFAULT_NAME = IMAGE_DIR + INTERNAL_DIR + "default.png";  
    public final static String DEFAULT_KEY = "__default__";  

    private static ImageRegistry imageRegistry = new ImageRegistry();
    private static String[] imageDirs;
    
    static {
        imageDirs = new String[0];
        String omnetppBitmapPath = System.getenv("OMNETPP_BITMAP_PATH");
        if (omnetppBitmapPath != null) 
            imageDirs = omnetppBitmapPath.split(";");
        // create and register a default / notfound image 
        imageRegistry.put(DEFAULT_KEY, 
                new NedImageDescriptor(ImageFactory.class, DEFAULT_NAME));
    }
    
    /**
     * Returns the requested image seraching on the bitmap path and in the resources
     * @param imageId the rquested image's id
     * @return Image with a given ID 
     */
    public static Image getImage(String imageId) {
        return getImage(imageId, null, null, -1);
    }

    public static ImageDescriptor getDescriptor(String imageId) {
        return getDescriptor(imageId, null, null, -1);
    }

    public static Image getImage(String imageId, String imageSize, RGB shade, int weight) {
        if (imageId == null) return null;
        // look for the image on filesystem/jar and return the full key to retrieve
        String key = getKeyFor(imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.get(key);
        // if key was null (ie not found) return the default image
        return imageRegistry.get(DEFAULT_KEY);
    }

    public static ImageDescriptor getDescriptor(String imageId, String imageSize, RGB shade, int weight) {
        if (imageId == null) return null;
        // look for the image on filesystem/jar and return the full key to retrieve
        String key = getKeyFor(imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.getDescriptor(key);
        // if key was null (ie not found) return the default image descriptor
        return imageRegistry.getDescriptor(DEFAULT_KEY);
    }
    
    /**
     * Returns an image key with a given id, with preferred size. The method also 
     * caches the image for future reuse
     * @param imageId The requested image name
     * @param imageSize preferred size if possible either vs,s,l,vl or a number as size in pixel
     * @param shade icon shading color 
     * @param weight weitght (0-100) of the colorization effect
     * @return the requested image or <code>null</code> if imageId was also <code>null</code>
     */
    private static String getKeyFor(String imageId, String imageSize, RGB shade, int weight) {
        String sizedId = imageId;
        int pixelSize = -1;
        // translate the textua size to pixel
        if(imageSize == null) {
            pixelSize = -1;
        } else if("vs".equals(imageSize.toLowerCase())) {
            sizedId += "_vs";
            pixelSize = 16;
        } else if("s".equals(imageSize.toLowerCase())) {
            sizedId += "_s";
            pixelSize = 20;
        } else if("l".equals(imageSize.toLowerCase())) {
            sizedId += "_l";
            pixelSize = 60;
        } else if("vl".equals(imageSize.toLowerCase())) {
            sizedId += "_vl";
            pixelSize = 100;
        } else  try {
                    pixelSize = Integer.valueOf(imageSize).intValue();
                } catch (Exception e) { }

         return getKeyFor(sizedId, pixelSize, shade, weight);
    }
 
    /**
     * Returns an image key with a given id, with preferred size and color. The method also 
     * caches the image for future reuse, so a get() with the returned key is guaranteed
     * to return an image.
     * @param baseId The requested image name
     * @param imageSize preferred size if possible
     * @param shade icon shading color 
     * @param weight weitght (0-100) of the colorization effect
     * @return the requested image key or <code>null</code> if baseId was also <code>null</code>
     */
    private static String getKeyFor(String baseId, int imageSize, RGB shade, int weight) {
        // if imageId is null (ie. no image) then we should return null
        // as an image
        if(baseId == null) return null;

        String decoratedImageId = baseId;
        // add the size param to the id
        if (imageSize > 0)
            decoratedImageId += "_" + imageSize;
        // add the color part
        if (shade != null && weight > 0)
            decoratedImageId += "_" + shade + "_" + weight;

        // try to get it from the registry (maybe we've already created it)
        ImageDescriptor result = imageRegistry.getDescriptor(decoratedImageId);
        if (result != null) 
            return decoratedImageId;
    
        // if the colorized image is not in the registry create a non colorized one
        result = createDescriptor(baseId);
        // return null if image not found
        if (result == null ) 
            return null;

        // adjust the colorization and size parameteres for the descriptor
        if(result instanceof NedImageDescriptor) {
            ((NedImageDescriptor)result).setColorization(shade);
            ((NedImageDescriptor)result).setColorizationWeight(weight);
            ((NedImageDescriptor)result).setPreferredWidth(imageSize);
        }
        
        // add it to the image registry, so later we can reuse it. The key provides the
        // name, horizontal size, and colorization data too
        imageRegistry.put(decoratedImageId, result);
        return decoratedImageId;
    }
    
        /**
     * Returns an image with a given id, with preferred size. The method also 
     * caches the image for future reuse
     * @param baseId The requested image name
     * @param size the requested horizontal size
     * @return the requested image or <code>null</code> if imageId was also <code>null</code>
     */
//    public static ImageDescriptor getDescriptor(String baseId) {
//        // if baseId is null (ie. no image) then we should return null
//        // as a descriptor
//        if (baseId == null) return null;
//
//        // try to get it from the registry (in case we've already created it)
//        ImageDescriptor result = imageRegistry.getDescriptor(baseId);
//        if (result != null) 
//            return result;
//    
//        // if the image descriptor is not in the registry look for it in the filesystem or JAR file
//        result = createDescriptor(baseId);
//        // if file was not found return the default not found icon
//        if (result == null)
//            return imageRegistry.getDescriptor(DEFAULT_KEY);
//
//        // add it to the image registry, so later we can reuse it. The key provides the
//        // name, horizontal size, and colorization data too
//        imageRegistry.put(baseId, result);
//        return result;
//    }

    /**
     * Looks for image files with the given name in the image search path. First looks for 
     * svg, then png and then gif 
     * @param baseName basic name of the figure
     * @return The file object describing for the given name
     */
    private static NedImageDescriptor createDescriptor(String baseName) {
        NedImageDescriptor result;
        // TODO svg support missing
        for(String currPath : imageDirs) 
            if ((result = createDescriptor(null, currPath, baseName, "png")) != null) 
                return result;
        // if not found in the filesystem, look for it in the JAR file
        if ((result = createDescriptor(ImageFactory.class, IMAGE_DIR, baseName, "png")) != null) 
            return result;
        // serach for gifs if no PNG found
        for(String currPath : imageDirs) 
            if ((result = createDescriptor(null, currPath, baseName, "gif")) != null) 
                return result;
        if ((result = createDescriptor(ImageFactory.class, IMAGE_DIR, baseName, "gif")) != null) 
            return result;
        
        return null;
    }
    
    /**
     * @param refClass Reference class, if <code>null</code> file is loaded from the filesystem
     *                 otehrwise from the same resource where the this class is found 
     * @param dir Base path for file
     * @param fileName The name of the file
     * @param ext extension to be used
     * @return The ImageDescriptor or <code>null</code> if does exist.
     */
    private static NedImageDescriptor createDescriptor(Class refClass, String dir, String fileName, String ext) {
        String name = (new Path(dir)).append(fileName).addFileExtension(ext).toString();
        NedImageDescriptor iDesc = new NedImageDescriptor(refClass, name);
        // check if the resource exists
        if(iDesc.canCreate()) return iDesc;
        
        return null;
    }
    
}
