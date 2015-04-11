/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.image;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.filesystem.EFS;
import org.eclipse.core.filesystem.IFileStore;
import org.eclipse.core.filesystem.URIUtil;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.Debug;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.StringUtils;

/**
 * Utility class for loading images.
 *
 * <p>Images can be stored in global or project scope. The search path of the global
 * scope contains the directory i.e.
 * in the images directory of the OMNeT++ installation (configured on the OMNeT++ preference page)
 * or in the images directory of the org.omnetpp.common plugin.
 * The search path of a project scope contains the images directory of the project and its referenced
 * projects, and the global search path.
 *
 * <p>Images are identified by a relative path (e.g. "device/pc"), and
 * you can optionally specify a scale factor ("s", "vs", "l", "vl"), an absolute
 * size, and a shading.
 *
 * <p>To load an image from the global scope use
 * <pre>
 * ImageFactory.global().getImage(imageId);
 * </pre>
 *
 * <p>To load an image from the local scope of {@code project} use
 * <pre>
 * ImageFactory.of(project).getImage(imageId);
 * </pre>
 *
 * TODO describe search order
 */
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
    public final static String CURSOR_IMAGE_DRAGGER = CURSOR_IMAGE_DIR + "dragger";

    public final static String UNKNOWN = INTERNAL_DIR + "unknown";
    public final static String DEFAULT = INTERNAL_DIR + "default";

    public final static String DEFAULT_NAME = IMAGE_DIR + INTERNAL_DIR + "default.png";
    public final static String DEFAULT_PIN = DECORATORS_IMAGE_DIR + "pin";
    public final static String DEFAULT_ERROR = DECORATORS_IMAGE_DIR + "error";
    public final static String DEFAULT_KEY = "__default__";

    private static final IPath[] EMPTY = new IPath[0];
    private static final IPath IMAGES_PATH = new Path(IMAGE_DIR);

    private static final String pathSeparator = ";"; // TODO File.pathSeparator?

    // tried file extensions in the order of preference
    private static final String[] fileExtensions = new String[] {"png", "gif"}; // TODO svg?

    // icon geometry
    private static final Padding ICON_PADDING = new Padding(1);
    private static final Scaling ICON_SIZE = new Scaling(16,16);

    // ImageFactory instances: one global and one per project
    private static ImageFactory globalFactory = new ImageFactory(EMPTY);
    private static Map<IProject, ImageFactory> projectFactories = new HashMap<IProject, ImageFactory>();

    // ImageRegistries are backed by a ResourceManager which maps image descriptors to images,
    // and prevents reloading an image if it is already loaded. We want to reload an image when
    // image file changed, therefore a version number is added to the image descriptor.
    // The version numbers of new variables are initialized from this global variable.
    // The variable is increased when an image file change is detected.
    private static int currentVersion = 0;

    // image search path: absolute path of directories
    final IPath[] imagePath;
    // the search continued in the fallback when not found in this cache
    ImageFactory fallback;
    // for images and icons
    ImageRegistry imageRegistry = new ImageRegistry(Display.getDefault());
    // name of png and gif images in imagePath
    List<String> imageNameList = null;
    // workspace folders in the imagePath
    IContainer[] workspaceFoldersInPath;

    /**
     * Sets the global image directories as file system paths.
     * Must be called BEFORE accessing the image factory.
     */
    public static void initialize() {
        updateGlobalImagePath();

        // create and register a default / not found image
        globalFactory.imageRegistry.put(DEFAULT_KEY, new NedImageDescriptor(ImageFactory.class, DEFAULT_NAME, currentVersion));

        // register listener for workspace changes
        ResourcesPlugin.getWorkspace().addResourceChangeListener(new IResourceChangeListener() {
            @Override
            public void resourceChanged(IResourceChangeEvent event) {
                handleResourceChangeEvent(event);
            }
        }, IResourceChangeEvent.POST_CHANGE);

        // register listener for OMNETPP_ROOT and OMNETPP_IMAGE_PATH changes
        IPreferenceStore preferences = CommonPlugin.getConfigurationPreferenceStore();
        preferences.addPropertyChangeListener(new IPropertyChangeListener() {
            @Override
            public void propertyChange(PropertyChangeEvent event) {
                if (event.getProperty().equals(IConstants.PREF_OMNETPP_IMAGE_PATH) ||
                        event.getProperty().equals(IConstants.PREF_OMNETPP_ROOT))
                    updateGlobalImagePath();
            }
        });
    }

    /**
     * Returns the global image factory.
     */
    public static ImageFactory global() {
        return globalFactory;
    }

    /**
     * Returns the image factory for {@code project}.
     */
    public static ImageFactory of(IProject project) {
        if (project == null)
            return globalFactory;
        ImageFactory cache = projectFactories.get(project);
        if (cache == null) {
            IContainer[] folders = getImagePathFor(project);
            IPath[] dirs = new IPath[folders.length];
            for (int i = 0; i < dirs.length; ++i)
                dirs[i] = folders[i].getLocation();
            cache = new ImageFactory(dirs, globalFactory);
            projectFactories.put(project, cache);
        }
        return cache;
    }

    /**
     * Returns the image path of the specified project.
     * The path will contain the <em>image</em> directory (if exists) of {@code project},
     * and of all of its dependencies.
     * The result won't contain the global search path; it is implicitly
     * appended to the project search path (and they directories can be outside the workspace).
     */
    public static IContainer[] getImagePathFor(IProject project) {
        try {
            Set<IContainer> result = new LinkedHashSet<IContainer>();
            IProject[] projects = ProjectUtils.getAllReferencedProjects(project, true, true);
            for (IProject p : projects)
                collectProjectImageDirs(p, result);
            return result.toArray(new IContainer[result.size()]);
        }
        catch (CoreException e) {
            return new IContainer[0];
        }
    }

    /**
     * Creates an ImageFactory with the specified search path.
     */
    ImageFactory(IPath[] imagePath) {
        this(imagePath, null);
    }

    /**
     * Creates a factory that searches in the specified image path,
     * and in a fallback factory.
     */
    ImageFactory(IPath[] imagePath, ImageFactory fallback) {
        Assert.isNotNull(imagePath);
        if (Debug.debug)
            Debug.println("ImageFactory: creating image cache for path: " + StringUtils.join(imagePath, pathSeparator));

        this.imagePath = imagePath;
        this.fallback = fallback;

        IWorkspace workspace = ResourcesPlugin.getWorkspace();
        List<IContainer> folders = new ArrayList<IContainer>();

        for (IPath dir : imagePath) {
            URI uri = URIUtil.toURI(dir);
            IContainer[] containers = workspace.getRoot().findContainersForLocationURI(uri);
            folders.addAll(Arrays.asList(containers));
        }

        workspaceFoldersInPath = folders.toArray(new IContainer[folders.size()]);
    }

    /**
     * The current image directories as a string array (file system path).
     * Do not change the returned value!
     */
    public IPath[] getImagePath() {
        return imagePath;
    }

    /**
     * Returns a (cached) 16x16 image with padding 1.
     */
    public Image getIconImage(String imageId) {
        return getImage(imageId, ICON_SIZE, ICON_PADDING, null, -1);
    }

    /**
     * Returns an image for a given ID. If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public Image getImage(String imageId) {
        return getImage(imageId, (Scaling)null, null, null, -1);
    }

    /**
     * Returns an image for a given ID, scaling and shading.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public Image getImage(String imageId, String scaling, RGB shade, int weight) {
        return getImage(imageId, Scaling.parse(scaling), null, shade, weight);
    }

    /**
     * Returns an image for a given ID, scaling, padding, and shading.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public Image getImage(String imageId, String scaling, Padding padding, RGB shade, int weight) {
        return getImage(imageId, Scaling.parse(scaling), padding, shade, weight);
    }

    /**
     * Returns an image for a given ID, scaling, padding, and shading.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public Image getImage(String imageId, Scaling scaling, Padding padding, RGB shade, int weight) {
        if (imageId == null || "".equals(imageId))
            return null;

        // look for the image on file system/jar and return the full key to retrieve
        String key = getKeyFor(imageId, scaling, padding, shade, weight);

        // if image was not found, look it up among the legacy icons
        if (key == null)
            key = getKeyFor(LEGACY_DIR+imageId, scaling, padding, shade, weight);

        // not found: return UNKNOWN image
        if (key == null)
            key = getKeyFor(UNKNOWN, scaling, padding, shade, weight);

        // if image was found, get it from the registry
        return key != null ? imageRegistry.get(key) : null;
    }

    /**
     * Returns an image descriptor for a given ID.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public NedImageDescriptor getDescriptor(String imageId) {
        return getDescriptor(imageId, (Scaling)null, null, null, -1);
    }

    /**
     * Returns an image descriptor for a given ID and scaling.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public NedImageDescriptor getDescriptor(String imageId, String scaling) {
        return getDescriptor(imageId, Scaling.parse(scaling), null, null, -1);
    }

    /**
     * Returns an image descriptor for a given ID, scaling, padding, and shading.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public NedImageDescriptor getDescriptor(String imageId, String scaling, Padding padding, RGB shade, int weight) {
        return getDescriptor(imageId, Scaling.parse(scaling), padding, shade, weight);
    }

    /**
     * Returns an image descriptor for a given ID, scaling, padding, and shading.
     * If imageId is empty or null, the function returns null.
     * If the image is not found, returns the "unknown" image (id=UNKNOWN).
     * You should NEVER modify the returned instance.
     */
    public NedImageDescriptor getDescriptor(String imageId, Scaling scaling, Padding padding, RGB shade, int weight) {
        if (imageId == null || "".equals(imageId))
            return null;

        // look for the image on file system/jar and return the full key to retrieve
        String key = getKeyFor(imageId, scaling, padding, shade, weight);

        // if image was not found, look it up among the legacy icons
        if (key == null)
            key = getKeyFor(LEGACY_DIR+imageId, scaling, padding, shade, weight);

        // not found: return UNKNOWN image descriptor
        if (key == null)
            key = getKeyFor(UNKNOWN, scaling, padding, shade, weight);

        // if image was found, get it from the registry
        return key != null ? (NedImageDescriptor)imageRegistry.getDescriptor(key) : null;
    }

    /**
     * Returns a sorted list of all image IDs in the image path (files with .gif .png extension).
     */
    public synchronized List<String> getImageNameList() {
        if (imageNameList != null)
            return imageNameList;

        Set<String> result = new HashSet<String>();
        for (IPath basedir : imagePath) {
            try {
                IFileStore baseStore = EFS.getStore(URIUtil.toURI(basedir).normalize());
                result.addAll(getNameList(baseStore, baseStore.toURI().toString().length()));
            } catch (CoreException e) { }
        }

        if (fallback != null) {
            result.addAll(fallback.getImageNameList());
        }

        List<String> orderedNames = new ArrayList<String>(result);
        Collections.sort(orderedNames);
        // store/cache for later use
        imageNameList = orderedNames;
        return orderedNames;
    }

    /**
     * Re-reads the cached image names.
     */
    public synchronized void rereadImageNameList() {
        imageNameList = null;
        if (fallback != null)
            fallback.rereadImageNameList();
        getImageNameList();
    }

    /**
     * Returns a sorted list of folder categories found in image name list.
     */
    public List<String> getCategories() {

        Set<String> uniqueFolders = new HashSet<String>();
        for (String name : getImageNameList())
            if (name.contains("/"))
                uniqueFolders.add(name.replaceFirst("/.*", "/"));

        if (fallback != null)
            uniqueFolders.addAll(fallback.getCategories());

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


    /**
     * Returns an image key with a given id, with preferred size. The method also
     * caches the image for future reuse.
     *
     * @param imageId The requested image name
     * @param scaling preferred scaling
     * @param shade icon shading color
     * @param weight weight (0-100) of the colorization effect
     * @return the requested image or {@code null} if the image file is not found
     */
    private String getKeyFor(String imageId, Scaling scaling, Padding padding, RGB shade, int weight) {
        // strip the size suffix from the imageId
        String scaleSuffix = Scaling.findFilenameSuffix(imageId);
        if (scaleSuffix.length() > 0)
            imageId = imageId.substring(0, imageId.length() - scaleSuffix.length() - 1); // -1 for removing '_' too

        // scaling takes precedence over the suffix in the imageId
        if (scaling == null)
            scaling = Scaling.parse(scaleSuffix);

        String key = createImageKey(imageId, scaling, padding, shade, weight);
        NedImageDescriptor descriptor = getOrCreateDescriptor(key, imageId, scaling, padding, shade, weight);
        return descriptor != null ? key : null;
    }

    /**
     * Returns an descriptor with a given id, with preferred scaling, padding, and color. The method also
     * caches the image for future reuse, so a get() with the returned key is guaranteed
     * to return an image.
     *
     * @param baseId The requested image name
     * @param scaling preferred scaling
     * @param shade icon shading color
     * @param weight weight (0-100) of the colorization effect
     * @return the requested image descriptor or {@code null} if the file is not found
     */
    private NedImageDescriptor getOrCreateDescriptor(String key, String baseId, Scaling scaling, Padding padding, RGB shade, int weight) {
        Assert.isNotNull(baseId);

        // try to get it from the registry (maybe we've already created it)
        NedImageDescriptor result = (NedImageDescriptor)imageRegistry.getDescriptor(key);
        if (result != null)
            return result;

        // if the colorized image is not in the registry create a non colorized one
        result = createDescriptor(baseId, scaling, padding, shade, weight);

        // if not found, then try fallback image factory
        if (result == null) {
            if (fallback != null)
                result = fallback.getOrCreateDescriptor(key, baseId, scaling, padding, shade, weight);
        }

        // add it to the image registry, so later we can reuse it. The key provides the
        // name, horizontal size, and colorization data too
        if (result != null)
            imageRegistry.put(key, result);
        return result;
    }

    /**
     * Looks for image files with the given name in the image search path. First looks for
     * png and then gif. If found it creates a descriptor with the specified attributes,
     * otherwise returns {@code null}.
     *
     * @param baseName basic name of the figure
     * @param scaling in percent
     * @return The file object describing for the given name
     */
    private NedImageDescriptor createDescriptor(String baseName, Scaling scaling, Padding padding, RGB shade, int weight) {
        NedImageDescriptor result;
        for (String extension : fileExtensions) {
            for (IPath dir : imagePath)
                if ((result = createDescriptor(null, dir, baseName, extension, scaling, padding, shade, weight)) != null)
                    return result;
            // if not found in the filesystem, look for it in the JAR file
            if (this == globalFactory)
                if ((result = createDescriptor(ImageFactory.class, IMAGES_PATH, baseName, extension, scaling, padding, shade, weight)) != null)
                    return result;
        }
        return null;
    }

    /**
     * Removes all images from the cache.
     */
    public synchronized void clearCache() {
        if (Debug.debug)
            Debug.println("ImageFactory: dropping images from cache: " + StringUtils.join(imagePath, pathSeparator));
        // do not dispose old registry, because images are used by widgets, figures, etc.
        imageRegistry = new ImageRegistry(Display.getDefault());
        if (imageNameList != null)
            imageNameList.clear();
    }


    /**
     * Adds the image directories of {@code project} to {@code result}.
     * Currently only the 'images' directory is checked.
     * TODO Add a project property for the location of image directories.
     */
    private static void collectProjectImageDirs(IProject project, Set<IContainer> result) {
        IFolder folder = project.getFolder("images");
        if (folder.exists() && folder.getLocation().toFile().exists())
            result.add(folder);
    }

    /**
     * Creates a key for the given image.
     * The returned value is used as the key in the ImageRegistry.
     */
    private static String createImageKey(String imageId, Scaling scaling, Padding padding, RGB shade, int weight) {
        String key = imageId;
        // add the size param to the id
        if (scaling != null)
            key += ":" + scaling.width + "," + scaling.height;
        if (padding != null && !padding.equals(Padding.NULL))
            key += ":" + padding.left + "," + padding.top + "," + padding.right + "," + padding.bottom;
        // add the color part
        if (shade != null && weight > 0)
            key += ":" + shade + ":" + weight;
        return key;
    }

    /**
     * Returns the descriptor of the specified image, or {@code null} if not found.
     * If {@code scaling} is relative and one of the standard sizes (40%, 60%, 100%, 150%, 250%),
     * then descriptor refers to the scaled image if exists. Otherwise it loads the normal image and
     * scales it to the requested size.
     *
     * @param refClass reference class, if {@code null} file is loaded from the file-system
     *                 otherwise from the same resource where this class is located
     * @param dir base path for file
     * @param baseName the name of the file
     * @param ext the file extension
     * @param scaling the scaling for the image
     * @param shade the colorization of the image
     * @param weight the weight of the colorization
     * @return the ImageDescriptor or {@code null} if does exist.
     */
    private static NedImageDescriptor createDescriptor(Class<?> refClass, IPath dir, String baseName, String ext, Scaling scaling, Padding padding, RGB shade, int weight) {
        if (dir == null)
            return null;

        // add a size suffix to the filename if we need some of the predefined scaling factors
        String sizeSuffix = scaling != null ? scaling.getFilenameSuffix() : "";

        if (!"".equals(sizeSuffix)) {
            // try to load the resized image
            String name = dir.append(baseName+sizeSuffix).addFileExtension(ext).toString();
            NedImageDescriptor iDesc = new NedImageDescriptor(refClass, name, currentVersion, Scaling.NORMAL, padding, shade, weight);
            // check if the resource exists
            if (iDesc.canCreate()) return iDesc;
        }

        // load the normal size image and scale it
        String name = dir.append(baseName).addFileExtension(ext).toString();
        NedImageDescriptor iDesc = new NedImageDescriptor(refClass, name, currentVersion, scaling, padding, shade, weight);
        // check if the resource exists
        if (iDesc.canCreate()) return iDesc;

        return null;
    }


    /**
     * Get the image ID for a file or image IDs if it is a directory (recursively).
     *
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
            if (!toAdd.endsWith(".gif") && !toAdd.endsWith(".png"))
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
     * This method is called after some resource changed in the workspace.
     * It is responsible for dropping stale information from the cache.
     *
     * <p> It does the following:
     * <ul>
     * <li> If some project is opened, closed, or deleted, then drop everything
     *      from the cache of the project and its dependent projects.
     * <li> If some project description is changed, then drop everything
     *      from the cache of the project and its dependent projects.
     * <li> If some content is changed in an image path, then drop everything
     *      from the cache of the project and its dependent projects.
     * <li> If some image file is changed in an image path, then increase
     *      currentVersion.
     * </ul>
     */
    private static void handleResourceChangeEvent(IResourceChangeEvent e) {
        int eventType = e.getType();

        if (eventType == IResourceChangeEvent.POST_CHANGE) {
            // check that some dependent project was opened/closed/deleted or
            //   its project description changed
            IResourceDelta delta = e.getDelta();
            final Set<IProject> projectsChanged = new HashSet<IProject>();
            try {
                delta.accept(new IResourceDeltaVisitor() {
                    @Override
                    public boolean visit(IResourceDelta delta) throws CoreException {
                        if (delta.getResource() instanceof IProject) {
                            if ((delta.getFlags() & (IResourceDelta.OPEN|IResourceDelta.REMOVED|IResourceDelta.DESCRIPTION)) != 0) {
                                projectsChanged.add((IProject)delta.getResource());
                            }
                            return false;
                        }
                        return true;
                    }
                });
            }
            catch (CoreException ex) {}

            for (IProject project : projectsChanged)
                invalidateCache(project);

            // check that there are some change in the image path
            Set<IProject> projectsInCache = new HashSet<IProject>(projectFactories.keySet());
            final boolean[] imageFileChanged = new boolean[1];
            for (IProject project : projectsInCache) {
                for (IContainer dir : ImageFactory.of(project).workspaceFoldersInPath) {
                    IResourceDelta dirDelta = delta.findMember(dir.getFullPath());
                    if (dirDelta != null) {
                        invalidateCache(project);
                        if (!imageFileChanged[0]) {
                            try {
                                dirDelta.accept(new IResourceDeltaVisitor() {
                                    @Override
                                    public boolean visit(IResourceDelta delta) throws CoreException {
                                        if (delta.getResource() instanceof IFile &&
                                                Arrays.asList(fileExtensions).contains(delta.getResource().getFullPath().getFileExtension().toLowerCase()) &&
                                                (delta.getFlags() & (IResourceDelta.CONTENT | IResourceDelta.REPLACED)) != 0)
                                            imageFileChanged[0] = true;
                                        return !imageFileChanged[0];
                                    }
                                });
                            }
                            catch (CoreException ex) {}
                        }
                        break;
                    }
                }
            }

            if (imageFileChanged[0]) {
                Debug.println("ImageFactory: increasing image version number");
                currentVersion++;
            }
        }
    }

    /**
     * Clear everything from the cache of the specified {@code project} and referencing
     * projects recursively. If {@code project} is {@code null}, then it clears
     * the global cache.
     */
    private static void invalidateCache(IProject project) {
        if (project != null) {
            if (projectFactories.remove(project) != null)
                Debug.println("ImageFactory: invalidated image cache of "+project);;
            for (IProject dependentProject : project.getReferencingProjects())
                invalidateCache(dependentProject);
        }
        else {
            globalFactory.clearCache();
            projectFactories.clear();
            Debug.println("ImageFactory: invalidated all image caches");
        }
    }

    /**
     * This method should be called after each change of the OMNETPP_IMAGE_PATH.
     */
    private static void updateGlobalImagePath() {
        IPreferenceStore preferences = CommonPlugin.getConfigurationPreferenceStore();
        IPath[] newImagePath = parseImagePath(preferences.getString(IConstants.PREF_OMNETPP_IMAGE_PATH));
        if (!Arrays.equals(globalFactory.imagePath, newImagePath)) {
            globalFactory = new ImageFactory(newImagePath);
            projectFactories.clear();
            if (Debug.debug)
                Debug.println("ImageFactory: global image path changed to " + StringUtils.join(newImagePath, pathSeparator));
        }
    }

    /**
     * Parses an OMNETPP_IMAGE_PATH value.
     */
    private static IPath[] parseImagePath(String path) {
        String[] dirs = path.split(pathSeparator);
        IPath[] imagePath = new IPath[dirs.length];
        for (int i = 0; i < dirs.length; ++i) {
            try {
                imagePath[i] = new Path(StringUtils.substituteVariables(dirs[i]));
            }
            catch (CoreException e) {
                imagePath[i] = new Path(dirs[i]);
            }
        }
        return imagePath;
    }
}
