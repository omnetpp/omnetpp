package org.omnetpp.common.image;

import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.filesystem.EFS;
import org.eclipse.core.filesystem.IFileStore;
import org.eclipse.core.filesystem.URIUtil;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.RGB;

public class ImageFactory {
    private final static String IMAGE_DIR = "/images/";
    private final static String INTERNAL_DIR = "_internal/";
    private final static String LEGACY_DIR = "old/";
    private final static String TOOL_IMAGE_DIR = INTERNAL_DIR + "toolbar/";
    private final static String MODEL_IMAGE_DIR = INTERNAL_DIR + "model/";
    private final static String DECORATORS_IMAGE_DIR = INTERNAL_DIR + "decorators/";

    public final static String TOOLBAR_IMAGE_UNPIN = TOOL_IMAGE_DIR + "unpin";
    public final static String TOOLBAR_IMAGE_LAYOUT = TOOL_IMAGE_DIR + "layout";
    public final static String TOOLBAR_IMAGE_OUTLINE = TOOL_IMAGE_DIR + "outline";
    public final static String TOOLBAR_IMAGE_ZOOM = TOOL_IMAGE_DIR + "zoom";
    public final static String TOOLBAR_IMAGE_HAND = TOOL_IMAGE_DIR + "hand";
    public final static String TOOLBAR_IMAGE_ZOOMMINUS = TOOL_IMAGE_DIR + "zoomminus";
    public final static String TOOLBAR_IMAGE_ZOOMPLUS = TOOL_IMAGE_DIR + "zoomplus";
    public final static String TOOLBAR_IMAGE_REFRESH = TOOL_IMAGE_DIR + "refresh";
    public final static String TOOLBAR_IMAGE_TEMPLATE = TOOL_IMAGE_DIR + "template";
    public final static String TOOLBAR_IMAGE_PLOT = TOOL_IMAGE_DIR + "plot";
    public final static String TOOLBAR_IMAGE_ADDFILTERTODATASET = TOOL_IMAGE_DIR + "AddFilterToDataset";
    public final static String TOOLBAR_IMAGE_ADDSELECTEDTODATASET = TOOL_IMAGE_DIR + "AddSelectedToDataset";

    public final static String MODEL_IMAGE_CHANNEL = MODEL_IMAGE_DIR + "Channel";
    public final static String MODEL_IMAGE_CHANNELINTERFACE = MODEL_IMAGE_DIR + "ChannelInterface";
    public final static String MODEL_IMAGE_COMPOUNDMODULE = MODEL_IMAGE_DIR + "CompoundModule";
    public final static String MODEL_IMAGE_INTERFACE = MODEL_IMAGE_DIR + "Interface";
    public final static String MODEL_IMAGE_CONNECTION = MODEL_IMAGE_DIR + "Connection";
    public final static String MODEL_IMAGE_GATE = MODEL_IMAGE_DIR + "Gate";
    public final static String MODEL_IMAGE_IMPORT = MODEL_IMAGE_DIR + "Import";
    public final static String MODEL_IMAGE_INVALID = MODEL_IMAGE_DIR + "Invalid";
    public final static String MODEL_IMAGE_PARAM = MODEL_IMAGE_DIR + "Param";
    public final static String MODEL_IMAGE_PROPERTY = MODEL_IMAGE_DIR + "Property";
    public final static String MODEL_IMAGE_SIMPLEMODULE = MODEL_IMAGE_DIR + "SimpleModule";
    public final static String MODEL_IMAGE_SUBMODULE = MODEL_IMAGE_DIR + "Submodule";
    public final static String MODEL_IMAGE_FOLDER = MODEL_IMAGE_DIR + "Folder";

    public final static String DECORATOR_IMAGE_ERROR = DECORATORS_IMAGE_DIR + "error";
    public final static String DECORATOR_IMAGE_WARNING = DECORATORS_IMAGE_DIR + "warning";

    public final static String DEFAULT_NAME = IMAGE_DIR + INTERNAL_DIR + "default.png";
    public final static String DEFAULT_PIN = DECORATORS_IMAGE_DIR + "pin";
    public final static String DEFAULT_KEY = "__default__";

    private static ImageRegistry imageRegistry = new ImageRegistry();
    private static String[] imageDirs;

    static {
        imageDirs = new String[0];
        String omnetppBitmapPath = System.getenv("OMNETPP_IMAGE_PATH");
        if (omnetppBitmapPath != null)
            imageDirs = omnetppBitmapPath.split(";");
        // create and register a default / notfound image
        imageRegistry.put(DEFAULT_KEY,
                new ColorizableImageDescriptor(ImageFactory.class, DEFAULT_NAME));

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
        if (imageId == null || "".equals(imageId)) return null;
        // look for the image on filesystem/jar and return the full key to retrieve
        String key = getKeyFor(imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.get(key);
        // if image was not found, look it up among the legacy icons
        key = getKeyFor(LEGACY_DIR+imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.get(key);
        // if key was null (ie not found) return the default image
        return imageRegistry.get(DEFAULT_KEY);
    }

    public static ImageDescriptor getDescriptor(String imageId, String imageSize, RGB shade, int weight) {
        if (imageId == null || "".equals(imageId)) return null;
        // look for the image on filesystem/jar and return the full key to retrieve
        String key = getKeyFor(imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.getDescriptor(key);
        // if image was not found, look it up among the legacy icons
        key = getKeyFor(LEGACY_DIR+imageId, imageSize, shade, weight);
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
        if(result instanceof ColorizableImageDescriptor) {
            ((ColorizableImageDescriptor)result).setColorization(shade);
            ((ColorizableImageDescriptor)result).setColorizationWeight(weight);
            ((ColorizableImageDescriptor)result).setPreferredWidth(imageSize);
        }

        // add it to the image registry, so later we can reuse it. The key provides the
        // name, horizontal size, and colorization data too
        imageRegistry.put(decoratedImageId, result);
        return decoratedImageId;
    }


    /**
     * Looks for image files with the given name in the image search path. First looks for
     * svg, then png and then gif
     * @param baseName basic name of the figure
     * @return The file object describing for the given name
     */
    private static ColorizableImageDescriptor createDescriptor(String baseName) {
        ColorizableImageDescriptor result;
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
     *                 otehrwise from the same resource where this class is located
     * @param dir Base path for file
     * @param fileName The name of the file
     * @param ext extension to be used
     * @return The ImageDescriptor or <code>null</code> if does exist.
     */
    private static ColorizableImageDescriptor createDescriptor(Class refClass, String dir, String fileName, String ext) {
        String name = (new Path(dir)).append(fileName).addFileExtension(ext).toString();
        ColorizableImageDescriptor iDesc = new ColorizableImageDescriptor(refClass, name);
        // check if the resource exists
        if(iDesc.canCreate()) return iDesc;

        return null;
    }


    /**
     * @return All image ID-s in the bitmap path (files with .gif .png .svg extension)
     */
    public static List<String> getImageNameList() {
    	Set<String> result = new HashSet<String>();
    	for(String basedir : imageDirs) {
			try {
				IFileStore baseStore = EFS.getStore(URIUtil.toURI(basedir).normalize());
	    		result.addAll(getNameList(baseStore, baseStore.toURI().toString().length()));
			} catch (CoreException e) {	}
    	}

    	// TODO add also the current and dependent project's own bitmap folder

    	// scan the resource directory for images
    	URL imagesResourceUrl= ImageFactory.class.getResource(IMAGE_DIR);
		try {
			// TODO beware that the toFileURL creates a copy of ALL icons if they are stored in a JAR file
			// this is sub optimal. It would be better if we could create a filesystem supporting
			// the bundleresource: protocol directly (ie we could read directy from the bundle)
			IFileStore resourceStore = EFS.getStore(FileLocator.toFileURL(imagesResourceUrl).toURI());
			result.addAll(getNameList(resourceStore, resourceStore.toURI().toString().length()));
		} catch (IOException e) {
		} catch (CoreException e) {
		} catch (URISyntaxException e) {
		}

    	List<String> orderedNames = new ArrayList<String>(result);
    	Collections.sort(orderedNames);
    	return orderedNames;
    }

    /**
     * Get the image ID for a file or image IDs if it is a directory (recursively(
     * @param fileStore A file or directory
     * @param stripBeginning (how much should be stripped at the beginning of the absolute path)
     * @return
     */
    private static Set<String> getNameList(IFileStore fileStore, int stripBeginning) {
    	Set<String> result = new HashSet<String>(1);
    	// check if this is a real file
    	if (fileStore.fetchInfo().exists() && !fileStore.fetchInfo().isDirectory()) {
    		String toAdd;
			toAdd = fileStore.toURI().toString().substring(stripBeginning);
			// convert backslash to slash on windows
			toAdd = toAdd.replace('\\','/');
			// strip the leading / -s if any
			while (toAdd.charAt(0) == '/')
					toAdd = toAdd.substring(1);
			// check if this is an image file, otherwise return with empty result
			if (!toAdd.endsWith(".gif") && !toAdd.endsWith(".png") && !toAdd.endsWith(".svg"))
				return result;

			// strip the extension
			toAdd = toAdd.substring(0, toAdd.length()-4);

			// look for size extensions and remove them
			if(toAdd.endsWith("_s") || toAdd.endsWith("_l"))
				toAdd = toAdd.substring(0, toAdd.length()-2);
			if(toAdd.endsWith("_vs") || toAdd.endsWith("_vl"))
				toAdd = toAdd.substring(0, toAdd.length()-3);

    		result.add(toAdd);
    	}

    	// if this is a directory (but not the internal dir)iterate through all contained files
    	if (fileStore.fetchInfo().isDirectory()
    			&& !fileStore.fetchInfo().getName().startsWith("_internal"))
			try {
				for(IFileStore childToAdd : fileStore.childStores(EFS.NONE, null)) {
					result.addAll(getNameList(childToAdd, stripBeginning));
				}
			} catch (CoreException e) { // do nothing if error occured
			}

    	return result;
    }

}
