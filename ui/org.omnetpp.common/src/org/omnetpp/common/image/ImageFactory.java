package org.omnetpp.common.image;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.filesystem.EFS;
import org.eclipse.core.filesystem.IFileStore;
import org.eclipse.core.filesystem.URIUtil;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;

public class ImageFactory {
    private final static String IMAGE_DIR = "/images/";
    private final static String INTERNAL_DIR = "_internal/";
    private final static String LEGACY_DIR = "old/";
    private final static String TOOL_IMAGE_DIR = INTERNAL_DIR + "toolbar/";
    private final static String MODEL_IMAGE_DIR = INTERNAL_DIR + "model/";
    private final static String DECORATORS_IMAGE_DIR = INTERNAL_DIR + "decorators/";
    public final static String EVENTLOG_IMAGE_DIR = INTERNAL_DIR + "eventlog/";
    public final static String CURSOR_IMAGE_DIR = INTERNAL_DIR + "cursor/";

    public final static String EVENLOG_IMAGE_BUBBLE = EVENTLOG_IMAGE_DIR + "Bubble";
    public final static String EVENLOG_IMAGE_EVENT_LOG_MESSAGE = EVENTLOG_IMAGE_DIR + "EventLogMessage";

    public final static String EVENLOG_IMAGE_MODULE_METHOD_BEGIN = EVENTLOG_IMAGE_DIR + "ModuleMethodBegin";
    public final static String EVENLOG_IMAGE_MODULE_METHOD_END = EVENTLOG_IMAGE_DIR + "ModuleMethodEnd";
    public final static String EVENLOG_IMAGE_MODULE_CREATED = EVENTLOG_IMAGE_DIR + "ModuleCreated";
    public final static String EVENLOG_IMAGE_MODULE_DELETED = EVENTLOG_IMAGE_DIR + "ModuleDeleted";
    public final static String EVENLOG_IMAGE_MODULE_REPARENTED = EVENTLOG_IMAGE_DIR + "ModuleReparented";
    public final static String EVENLOG_IMAGE_MODULE_DISPLAY_STRING_CHANGED = EVENTLOG_IMAGE_DIR + "ModuleDisplayStringChanged";

    public final static String EVENLOG_IMAGE_CONNECTION_CREATED = EVENTLOG_IMAGE_DIR + "ConnectionCreated";
    public final static String EVENLOG_IMAGE_CONNECTION_DELETED = EVENTLOG_IMAGE_DIR + "ConnectionDeleted";
    public final static String EVENLOG_IMAGE_CONNECTION_DISPLAY_STRING_CHANGED = EVENTLOG_IMAGE_DIR + "ConnectionDisplayStringChanged";

    public final static String EVENLOG_IMAGE_EVENT = EVENTLOG_IMAGE_DIR + "Event";
    public final static String EVENLOG_IMAGE_SELF_EVENT = EVENTLOG_IMAGE_DIR + "SelfEvent";
    public final static String EVENLOG_IMAGE_CANCEL_EVENT = EVENTLOG_IMAGE_DIR + "CancelEvent";

    public final static String EVENLOG_IMAGE_BEGIN_SEND = EVENTLOG_IMAGE_DIR + "BeginSend";
    public final static String EVENLOG_IMAGE_END_SEND = EVENTLOG_IMAGE_DIR + "EndSend";
    public final static String EVENLOG_IMAGE_SCHEDULE_AT = EVENTLOG_IMAGE_DIR + "ScheduleAt";
    public final static String EVENLOG_IMAGE_SEND_DIRECT = EVENTLOG_IMAGE_DIR + "SendDirect";
    public final static String EVENLOG_IMAGE_SEND_HOP = EVENTLOG_IMAGE_DIR + "SendHop";
    public final static String EVENLOG_IMAGE_DELETE_MESSAGE = EVENTLOG_IMAGE_DIR + "DeleteMessage";

    public final static String TOOLBAR_IMAGE_UNPIN = TOOL_IMAGE_DIR + "unpin";
    public final static String TOOLBAR_IMAGE_PROPERTIES = TOOL_IMAGE_DIR + "properties";
    public final static String TOOLBAR_IMAGE_LAYOUT = TOOL_IMAGE_DIR + "layout";
    public final static String TOOLBAR_IMAGE_OUTLINE = TOOL_IMAGE_DIR + "outline";
    public final static String TOOLBAR_IMAGE_ZOOM = TOOL_IMAGE_DIR + "zoom";
    public final static String TOOLBAR_IMAGE_HAND = TOOL_IMAGE_DIR + "hand";
    public final static String TOOLBAR_IMAGE_ZOOMMINUS = TOOL_IMAGE_DIR + "zoomminus";
    public final static String TOOLBAR_IMAGE_ZOOMPLUS = TOOL_IMAGE_DIR + "zoomplus";
    public final static String TOOLBAR_IMAGE_ZOOMTOFIT = TOOL_IMAGE_DIR + "zoomtofit";
    public final static String TOOLBAR_IMAGE_REFRESH = TOOL_IMAGE_DIR + "refresh";
    public final static String TOOLBAR_IMAGE_RENAME = TOOL_IMAGE_DIR + "rename";
    public final static String TOOLBAR_IMAGE_CHOOSEICON = TOOL_IMAGE_DIR + "chooseicon";
    public final static String TOOLBAR_IMAGE_TEMPLATE = TOOL_IMAGE_DIR + "template";
    public final static String TOOLBAR_IMAGE_OPEN = TOOL_IMAGE_DIR + "open";
    public final static String TOOLBAR_IMAGE_PLOT = TOOL_IMAGE_DIR + "plot";
    public final static String TOOLBAR_IMAGE_FILTER = TOOL_IMAGE_DIR + "filter";
    public final static String TOOLBAR_IMAGE_SEARCH = TOOL_IMAGE_DIR + "Search";
    public final static String TOOLBAR_IMAGE_SEARCH_NEXT = TOOL_IMAGE_DIR + "SearchNext";
    public final static String TOOLBAR_IMAGE_TOGGLE_BOOKMARK = TOOL_IMAGE_DIR + "bkmrk_nav";
    public final static String TOOLBAR_IMAGE_DISPLAY_MODE = TOOL_IMAGE_DIR + "displaymode";
    public final static String TOOLBAR_IMAGE_ADDFILTERTODATASET = TOOL_IMAGE_DIR + "AddFilterToDataset";
    public final static String TOOLBAR_IMAGE_ADDSELECTEDTODATASET = TOOL_IMAGE_DIR + "AddSelectedToDataset";
    public final static String TOOLBAR_IMAGE_SHOWSCALARS = TOOL_IMAGE_DIR + "ShowScalars";
    public final static String TOOLBAR_IMAGE_SHOWVECTORS = TOOL_IMAGE_DIR + "ShowVectors";
    public final static String TOOLBAR_IMAGE_SHOWHISTOGRAMS = TOOL_IMAGE_DIR + "ShowHistograms";
    public final static String TOOLBAR_IMAGE_EXPORTIMAGE = TOOL_IMAGE_DIR + "ExportImage";
    public final static String TOOLBAR_IMAGE_LEGEND = TOOL_IMAGE_DIR + "legend";

    public final static String MODEL_IMAGE_CHANNEL = MODEL_IMAGE_DIR + "Channel";
    public final static String MODEL_IMAGE_CHANNELINTERFACE = MODEL_IMAGE_DIR + "ChannelInterface";
    public final static String MODEL_IMAGE_COMPOUNDMODULE = MODEL_IMAGE_DIR + "CompoundModule";
    public final static String MODEL_IMAGE_NETWORK = MODEL_IMAGE_DIR + "Network";
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

    public final static String CURSOR_IMAGE_ZOOMIN = CURSOR_IMAGE_DIR + "ZoomIn";
    public final static String CURSOR_IMAGE_ZOOMOUT = CURSOR_IMAGE_DIR + "ZoomOut";

    public final static String UNKNOWN = INTERNAL_DIR + "unknown";
    public final static String DEFAULT = INTERNAL_DIR + "default";

    public final static String DEFAULT_NAME = IMAGE_DIR + INTERNAL_DIR + "default.png";
    public final static String DEFAULT_PIN = DECORATORS_IMAGE_DIR + "pin";
    public final static String DEFAULT_ERROR = DECORATORS_IMAGE_DIR + "error";
    public final static String DEFAULT_KEY = "__default__";

    // for non rescaled icons
    private static ImageRegistry imageRegistry = new ImageRegistry(Display.getDefault());
    // for 16x16 rescaled icons
    private static ImageRegistry iconImageRegistry = new ImageRegistry(Display.getDefault());
    private static String[] imageDirs;
    private static List<String> imageNameList = null;
    // image size constants
    public static final int SIZE_VS = -40;
    public static final int SIZE_S = -60;
    public static final int SIZE_NORMAL = -100;
    public static final int SIZE_L = -150;
    public static final int SIZE_VL = -250;

    static {
        // default is the "images" directory in the main omnetpp directory
        String defaultImageDir = new Path(Platform.getInstallLocation().getURL().getFile()).append("../images").toString();
        imageDirs = new String[] { defaultImageDir };

        // if image path is specified we should use that one instead of ../images
        String omnetppBitmapPath = System.getenv("OMNETPP_IMAGE_PATH");
        if (omnetppBitmapPath != null)
            imageDirs = omnetppBitmapPath.split(";");
        // create and register a default / not found image
        imageRegistry.put(DEFAULT_KEY,
                new NedImageDescriptor(ImageFactory.class, DEFAULT_NAME));

    }

    /**
     * @param imageId
     * @return a 16x16 image or null if there is no icon for that id
     */
    public static Image getIconImage(String imageId) {
        if (imageId == null)
            return null;
        // first try to get it from the icon cache
        Image img = iconImageRegistry.get(imageId);
        if (img != null)
            return img;
        // if not in the cache get it from the general icon cache and resize it to fit our needs
        NedImageDescriptor descr = getDescriptor(imageId);
        if (descr == null) return null;
        // Get a copy and re-sample the image to 16x16 with 1 pixel padding
        descr = descr.getCopy();
        descr.setPadding(1);
        descr.setPaddedSize(new Dimension(16,16));
        // store the descriptor for later re-use
        iconImageRegistry.put(imageId, descr);
        return descr.createImage();
    }

    /**
     * Returns the requested image searching on the bitmap path and in the resources.
     * You should NEVER modify the returned instance.
     * @param imageId the requested image's id
     * @return Image with a given ID
     */
    public static Image getImage(String imageId) {
        return getImage(imageId, null, null, -1);
    }

    /**
     * Returns an image descriptor for a given ID. You should NEVER modify the returned instance.
     * @param imageId
     * @return
     */
    public static NedImageDescriptor getDescriptor(String imageId) {
        return getDescriptor(imageId, null, null, -1);
    }

    public static Image getImage(String imageId, String imageSize, RGB shade, int weight) {
        if (imageId == null || "".equals(imageId)) return null;
        // look for the image on file system/jar and return the full key to retrieve
        String key = getKeyFor(imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.get(key);
        // if image was not found, look it up among the legacy icons
        key = getKeyFor(LEGACY_DIR+imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.get(key);
        // if image was not found, display the unknown icon
        key = getKeyFor(UNKNOWN, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return imageRegistry.get(key);
        // if key was null (ie. not found) return the default image
        return imageRegistry.get(DEFAULT_KEY);
    }

    public static NedImageDescriptor getDescriptor(String imageId, String imageSize, RGB shade, int weight) {
        if (imageId == null || "".equals(imageId)) return null;
        // look for the image on file system/jar and return the full key to retrieve
        String key = getKeyFor(imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return (NedImageDescriptor)imageRegistry.getDescriptor(key);
        // if image was not found, look it up among the legacy icons
        key = getKeyFor(LEGACY_DIR+imageId, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return (NedImageDescriptor)imageRegistry.getDescriptor(key);
        // if image was not found, display the unknown icon
        key = getKeyFor(UNKNOWN, imageSize, shade, weight);
        // if image was found, get it from the registry
        if (key != null) return (NedImageDescriptor)imageRegistry.getDescriptor(key);
        // if key was null (ie. not found) return the default image descriptor
        return (NedImageDescriptor)imageRegistry.getDescriptor(DEFAULT_KEY);
    }

    /**
     * Returns an image key with a given id, with preferred size. The method also
     * caches the image for future reuse
     * @param imageId The requested image name
     * @param imageSize preferred size if possible either vs,s,l,vl or a number as size in pixel or percent (as a negative number)
     * @param shade icon shading color
     * @param weight weight (0-100) of the colorization effect
     * @return the requested image or <code>null</code> if imageId was also <code>null</code>
     */
    private static String getKeyFor(String imageId, String imageSize, RGB shade, int weight) {
        String sizeSuffix = "";
        // strip the size suffix from the imageId
        if (imageId.endsWith("_s") || imageId.endsWith("_l")) {
            sizeSuffix = imageId.substring(imageId.length()-1);
            imageId = imageId.substring(0, imageId.length()-2);
        }
        if (imageId.endsWith("_vs") || imageId.endsWith("_vl")) {
            sizeSuffix = imageId.substring(imageId.length()-2);
            imageId = imageId.substring(0, imageId.length()-3);
        }

        // imageSIze takes precedence over the suffix in the imageId
        if (imageSize == null || "".equals(imageSize))
            imageSize = sizeSuffix;

        int imageScaling = SIZE_NORMAL;
        // translate the textual size to pixel
        if (imageSize == null || "".equals(imageSize) ) {
            imageScaling = SIZE_NORMAL;
        }
        else if ("vs".equals(imageSize.toLowerCase())) {
            imageScaling = SIZE_VS;
        }
        else if ("s".equals(imageSize.toLowerCase())) {
            imageScaling = SIZE_S;
        }
        else if ("l".equals(imageSize.toLowerCase())) {
            imageScaling = SIZE_L;
        }
        else if ("vl".equals(imageSize.toLowerCase())) {
            imageScaling = SIZE_VL;
        }
        else  try {
                    imageScaling = Integer.valueOf(imageSize).intValue();
                } catch (Exception e) { }

         return getKeyFor(imageId, imageScaling, shade, weight);
    }

    /**
     * Returns an image key with a given id, with preferred size and color. The method also
     * caches the image for future reuse, so a get() with the returned key is guaranteed
     * to return an image.
     * @param baseId The requested image name
     * @param imageSize preferred size if possible
     * @param shade icon shading color
     * @param weight weight (0-100) of the colorization effect
     * @return the requested image key or <code>null</code> if baseId was also <code>null</code>
     */
    private static String getKeyFor(String baseId, int imageSize, RGB shade, int weight) {
        // if imageId is null (ie. no image) then we should return null
        // as an image
        if (baseId == null) return null;

        String decoratedImageId = baseId;
        // add the size param to the id
        if (imageSize != SIZE_NORMAL)
            decoratedImageId += ":" + imageSize;
        // add the color part
        if (shade != null && weight > 0)
            decoratedImageId += ":" + shade + ":" + weight;

        // try to get it from the registry (maybe we've already created it)
        NedImageDescriptor result = (NedImageDescriptor)imageRegistry.getDescriptor(decoratedImageId);
        if (result != null)
            return decoratedImageId;

        // if the colorized image is not in the registry create a non colorized one
        result = createDescriptor(baseId, imageSize);
        // return null if image not found
        if (result == null )
            return null;

        // adjust the colorization and size parameters for the descriptor
        if (result instanceof NedImageDescriptor) {
            result.setColorization(shade);
            result.setColorizationWeight(weight);
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
     * @param preferredSize in percent
     * @return The file object describing for the given name
     */
    private static NedImageDescriptor createDescriptor(String baseName, int preferredSize) {
        NedImageDescriptor result;
        // TODO svg support missing
        for (String currPath : imageDirs)
            if ((result = createDescriptor(null, currPath, baseName, "png", preferredSize)) != null)
                return result;
        // if not found in the filesystem, look for it in the JAR file
        if ((result = createDescriptor(ImageFactory.class, IMAGE_DIR, baseName, "png", preferredSize)) != null)
            return result;
        // serach for gifs if no PNG found
        for (String currPath : imageDirs)
            if ((result = createDescriptor(null, currPath, baseName, "gif", preferredSize)) != null)
                return result;
        if ((result = createDescriptor(ImageFactory.class, IMAGE_DIR, baseName, "gif", preferredSize)) != null)
            return result;

        return null;
    }

    /**
     * @param refClass Reference class, if <code>null</code> file is loaded from the file-system
     *                 otherwise from the same resource where this class is located
     * @param dir Base path for file
     * @param fileName The name of the file
     * @param ext extension to be used
     * @param preferredSize The preferred size for the image (will add the needed suffix to the filename if necessar
     *        (_l,_vl,_s,_vs)
     * @return The ImageDescriptor or <code>null</code> if does exist.
     */
    private static NedImageDescriptor createDescriptor(Class<?> refClass, String dir, String fileName, String ext, int preferredSize) {
        // add a size suffix to the filename if we need some of the predefined scaling factors
        String sizeSuffix = "";
        if (preferredSize == SIZE_VS)
            sizeSuffix = "_vs";
        else if (preferredSize == SIZE_S)
            sizeSuffix = "_s";
        else if (preferredSize == SIZE_L)
            sizeSuffix = "_l";
        else if (preferredSize == SIZE_VL)
            sizeSuffix = "_vl";

        if (!"".equals(sizeSuffix)) {
            // try to load the resized image
            String name = (new Path(dir)).append(fileName+sizeSuffix).addFileExtension(ext).toString();
            NedImageDescriptor iDesc = new NedImageDescriptor(refClass, name, SIZE_NORMAL);
            // check if the resource exists
            if (iDesc.canCreate()) return iDesc;
        }

        // load the normal size image and scale it
        String name = (new Path(dir)).append(fileName).addFileExtension(ext).toString();
        NedImageDescriptor iDesc = new NedImageDescriptor(refClass, name, preferredSize);
        // check if the resource exists
        if (iDesc.canCreate()) return iDesc;

        return null;
    }


    /**
     * @return All image ID-s in the bitmap path (files with .gif .png .svg extension)
     */
    public static synchronized List<String> getImageNameList() {
        if (imageNameList != null)
            return imageNameList;

    	Set<String> result = new HashSet<String>();
    	for (String basedir : imageDirs) {
			try {
				IFileStore baseStore = EFS.getStore(URIUtil.toURI(basedir).normalize());
	    		result.addAll(getNameList(baseStore, baseStore.toURI().toString().length()));
			} catch (CoreException e) {	}
    	}

    	// TODO add also the current and dependent project's own bitmap folder

    	List<String> orderedNames = new ArrayList<String>(result);
    	Collections.sort(orderedNames);
    	// store/cache for later use
    	imageNameList = orderedNames;
    	return orderedNames;
    }

    /**
     * Re-reads the cached image names.
     */
    public static void rereadImageNameList() {
        imageNameList = null;
        getImageNameList();
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
			if (toAdd.endsWith("_s") || toAdd.endsWith("_l"))
				toAdd = toAdd.substring(0, toAdd.length()-2);
			if (toAdd.endsWith("_vs") || toAdd.endsWith("_vl"))
				toAdd = toAdd.substring(0, toAdd.length()-3);

    		result.add(toAdd);
    	}

    	// if this is a directory (but not the internal directory)iterate through all contained files
    	if (fileStore.fetchInfo().isDirectory()
    			&& !fileStore.fetchInfo().getName().startsWith("_internal"))
			try {
				for (IFileStore childToAdd : fileStore.childStores(EFS.NONE, null)) {
					result.addAll(getNameList(childToAdd, stripBeginning));
				}
			} catch (CoreException e) { // do nothing if error occurred
			}

    	return result;
    }

    /**
     * @return The folder categories found in image name list
     */
    public static List<String> getCategories() {

        Set<String> uniqueFolders = new HashSet<String>();
        for (String name : getImageNameList())
            if (name.contains("/"))
                uniqueFolders.add(name.replaceFirst("/.*", "/"));
        String[] folders = uniqueFolders.toArray(new String[]{});
        Arrays.sort(folders);
        return Arrays.asList(folders);
    }

    /**
     * Creates an image file containing {@code image} in
     * the given {@code format} in the tmp directory.
     *
     * @param imageName the name of image file (without directory)
     * @param image the image to be saved
     * @param format see {@link ImageLoader} for supported values
     * @return the absolute path of the created image file
     * @throws IOException if the temp directory cannot be read or the file cannot be written
     */
    // XXX clean up
    public static String createTemporaryImageFile(String imageName, Image image, int format)
    	throws IOException {

    	String tempDir = System.getProperty("java.io.tmpdir");
    	File tempFile = new File(tempDir, imageName);
    	ImageLoader loader = new ImageLoader();
    	loader.data = new ImageData[] { image.getImageData() };
    	loader.save(tempFile.getCanonicalPath(), format);
    	return tempFile.getCanonicalPath();
    }
}
