/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.wizard;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;

import org.apache.commons.collections.CollectionUtils;
import org.apache.commons.lang3.text.WordUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.wizard.IWizard;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.LicenseUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.wizard.support.FileUtils;
import org.omnetpp.common.wizard.support.LangUtils;
import org.omnetpp.common.wizard.support.ProcessUtils;

import freemarker.cache.StringTemplateLoader;
import freemarker.core.Environment;
import freemarker.ext.beans.BeansWrapper;
import freemarker.template.Configuration;
import freemarker.template.Template;
import freemarker.template.TemplateException;
import freemarker.template.TemplateModelException;

/**
 * The default implementation of IContentTemplate, using FreeMarker templates.
 * The createCustomPages() method is left undefined, but subclasses could use
 * XSWTWizardPage for implementing it.
 *
 * @author Andras
 */
public abstract class ContentTemplate implements IContentTemplate {
    public static final String CONTRIBUTORS_EXTENSIONPOINT_ID = "org.omnetpp.common.wizard.templatecontributors";

    private static final String SETOUTPUT_MARKER = "@@@@ setoutput \"${path}\" @@@@\n";
    private static final String SETOUTPUT_PATTERN = "(?s)@@@@ setoutput \"(.*?)\" @@@@\n"; // path must be matched as $1

    // template attributes
    private String name;
    private String description;
    private String category;
    private Image image;
    private boolean isDefault;

    // we need this ClassLoader for the Class.forName() calls in for both FreeMarker and XSWT.
    // Without it, templates (BeanWrapper) and XSWT would only be able to access classes
    // from the eclipse plug-in from which their code was loaded. In contrast, we want them to
    // have access to classes defined in this plug-in (FileUtils, IDEUtils, etc), and also
    // to the contents of jars loaded at runtime from the user's projects in the workspace.
    // See e.g. freemarker.template.utility.ClassUtil.forName(String)
    private ClassLoader classLoader = null;

    // FreeMarker configuration (stateless)
    private Configuration freemarkerConfiguration = null;

    // contributors list
    private List<IContentTemplateContributor> contributors;

    public ContentTemplate() {
    }

    public ContentTemplate(String name, String category, String description) {
        super();
        this.name = name;
        this.category = category;
        this.description = description;
    }

    public ContentTemplate(String name, String category, String description, Image image) {
        this(name, category, description);
        this.image = image;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getCategory() {
        return category;
    }

    public void setCategory(String category) {
        this.category = category;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public Image getImage() {
        return image;
    }

    public void setImage(Image image) {
        this.image = image;
    }

    public boolean getIsDefault() {
        return isDefault;
    }

    public void setIsDefault(boolean isDefault) {
        this.isDefault = isDefault;
    }

    public CreationContext createContextFor(IContainer folder, IWizard wizard, String wizardType) {
        // need to install our class loader while createContext runs: pre-registering classes
        // with StaticModel (Math, FileUtils, StringUtils, etc) needs this;
        // see freemarker.template.utility.ClassUtil.forName(String)
        ClassLoader oldClassLoader = Thread.currentThread().getContextClassLoader();
        Thread.currentThread().setContextClassLoader(getClassLoader());
        try {
            return createContext(folder, wizard, wizardType);
        }
        finally {
            Thread.currentThread().setContextClassLoader(oldClassLoader);
        }
    }

    protected CreationContext createContext(IContainer folder, IWizard wizard, String wizardType) {
        CreationContext context = new CreationContext(this, folder, wizard);

        // pre-register some potentially useful template variables
        Map<String, Object> variables = context.getVariables();
        variables.put("wizardType", wizardType);
        variables.put("templateName", name);
        variables.put("templateDescription", description);
        variables.put("templateCategory", category);
        if (folder != null) {
            // note: folder is null only for the export wizard; all other wizards should have a folder given
            variables.put("targetFolder", folder.getFullPath().toString());
            variables.put("rawProjectName", folder.getProject().getName());
            String projectNameIdent = StringUtils.makeValidIdentifier(folder.getProject().getName());
            variables.put("projectName", StringUtils.capitalize(projectNameIdent));
            variables.put("ProjectName", StringUtils.capitalize(projectNameIdent));
            variables.put("projectname", StringUtils.lowerCase(projectNameIdent));
            variables.put("PROJECTNAME", StringUtils.upperCase(projectNameIdent));
        }
        Calendar cal = Calendar.getInstance();
        variables.put("date", cal.get(Calendar.YEAR)+"-"+cal.get(Calendar.MONTH)+"-"+cal.get(Calendar.DAY_OF_MONTH));
        variables.put("year", ""+cal.get(Calendar.YEAR));
        variables.put("author", System.getProperty("user.name"));
        String licenseCode = LicenseUtils.getDefaultLicense();
        String licenseProperty = licenseCode.equals(LicenseUtils.NONE) ? "" : licenseCode; // @license(custom) is needed, so that wizards knows whether to create banners
        variables.put("licenseCode", licenseProperty); // for @license in package.ned
        variables.put("licenseText", LicenseUtils.getLicenseNotice(licenseCode));
        variables.put("bannerComment", LicenseUtils.getBannerComment(licenseCode, "//"));

        // open access to more facilities
        variables.put("creationContext", context);
        variables.put("classes", BeansWrapper.getDefaultInstance().getStaticModels());

        // make Math, FileUtils, StringUtils and static methods of other classes available to the template
        // see chapter "Bean wrapper" in the FreeMarker manual
        try {
            variables.put("Math", BeansWrapper.getDefaultInstance().getStaticModels().get(Math.class.getName()));
            variables.put("FileUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(FileUtils.class.getName()));
            variables.put("StringUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(StringUtils.class.getName()));
            variables.put("WordUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(WordUtils.class.getName()));
            variables.put("CollectionUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(CollectionUtils.class.getName()));
            variables.put("LangUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(LangUtils.class.getName()));
            variables.put("ProcessUtils", BeansWrapper.getDefaultInstance().getStaticModels().get(ProcessUtils.class.getName()));
        }
        catch (TemplateModelException e1) {
            CommonPlugin.logError(e1);
        }

        // allow contributors to add more vars, register further classes, etc.
        for (IContentTemplateContributor contributor : getContributors()) {
            try {
                contributor.contributeToContext(context);
            }
            catch (Exception e) {
                CommonPlugin.logError("Content template contributor threw an exception", e);
            }
        }
        return context;
    }

    protected void assertContextFolder(CreationContext context) {
        if (context.getFolder() == null)
            throw new IllegalStateException("This method cannot be used when context.getFolder() is null");
    }

    public ClassLoader getClassLoader() {
        if (classLoader == null)
            classLoader = createClassLoader();
        return classLoader;
    }

    protected ClassLoader createClassLoader() {
        // put all contributors' class loaders into the "classpath"
        List<IContentTemplateContributor> contributors = getContributors();
        ClassLoader[] loaders = new ClassLoader[contributors.size()];
        for (int i=0; i<loaders.length; i++)
            loaders[i] = contributors.get(i).getClass().getClassLoader();
        return new DelegatingClassLoader(loaders);
    }

    protected List<IContentTemplateContributor> getContributors() {
        if (contributors == null)
            contributors = createContributors();
        return contributors;
    }

    protected List<IContentTemplateContributor> createContributors() {
        List<IContentTemplateContributor> result = new ArrayList<IContentTemplateContributor>();
        try {
            IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(CONTRIBUTORS_EXTENSIONPOINT_ID);
            for (IConfigurationElement e : config) {
                Assert.isTrue(e.getName().equals("contributor"));
                IContentTemplateContributor contributor = (IContentTemplateContributor) e.createExecutableExtension("class");
                result.add(contributor);
            }
        } catch (Exception ex) {
            CommonPlugin.logError("Error instantiating content template contributors", ex);
        }
        return result;
    }

    protected Configuration getFreemarkerConfiguration() {
        if (freemarkerConfiguration == null)
            freemarkerConfiguration = createFreemarkerConfiguration();
        return freemarkerConfiguration;
    }

    protected Configuration createFreemarkerConfiguration() {
        // note: subclasses should override to add a real template loader
        Configuration cfg = new Configuration();

        cfg.setNumberFormat("computer"); // prevent digit grouping with comma

        // add loader for built-in macro definitions
        final String BUILTINS = "[builtins]";
        cfg.addAutoInclude(BUILTINS);
        String builtins =
            "<#macro do arg></#macro>" + // allow void methods to be called as: <@do object.setFoo(x)!>
            "<#macro setoutput path>\n" +
            SETOUTPUT_MARKER +
            "</#macro>\n" +
            "\n" +
            "<#function iif condition truevalue falsevalue=\"\">\n" +
            "    <#if condition><#return truevalue><#else><#return falsevalue></#if>\n" +
            "</#function>\n" +
            "\n";

        for (IContentTemplateContributor contributor : getContributors()) {
            try {
                String code = contributor.getAdditionalTemplateCode();
                if (!StringUtils.isEmpty(code))
                    builtins += "<#-- " + contributor.getClass().getName() + ": -->\n" + code + "\n\n";
            }
            catch (Exception e) {
                CommonPlugin.logError("Content template contributor threw an exception", e);
            }
        }

        StringTemplateLoader loader = new StringTemplateLoader();
        loader.putTemplate(BUILTINS, builtins);
        cfg.setTemplateLoader(loader);

        return cfg;
    }

    /**
     * Resolves references to other variables. Should be called from doPerformFinish() before
     * actually processing the templates.
     *
     * Some variables contain references to other variables (e.g.
     * "org.example.${projectname}"); substitute them.
     */
    protected void substituteNestedVariables(CreationContext context) throws CoreException {
        Map<String, Object> variables = context.getVariables();
        try {
            for (String key : variables.keySet()) {
                Object value = variables.get(key);
                if (value instanceof String) {
                    String newValue = evaluate((String)value, variables);
                    variables.put(key, newValue);
                }
            }
        } catch (Exception e) {
            throw CommonPlugin.wrapIntoCoreException(e);
        }
    }

    /**
     * Performs multiple passes of template processing on the given string, and returns the result.
     * Processing stops when the string is no longer changed by template processing, but at most
     * 100 passes are done.
     */
    public String evaluate(String value, Map<String, Object> variables) throws TemplateException {
        // classLoader stuff -- see freemarker.template.utility.ClassUtil.forName(String)
        ClassLoader oldClassLoader = Thread.currentThread().getContextClassLoader();
        Thread.currentThread().setContextClassLoader(getClassLoader());

        try {
            Configuration cfg = getFreemarkerConfiguration();
            int k = 0;
            while (true) {
                Template template = new Template("text" + k++, new StringReader(value), cfg, "utf8");
                StringWriter writer = new StringWriter();
                template.process(variables, writer);
                String newValue = writer.toString();
                if (value.equals(newValue) || k >= 100)
                    return newValue;
                value = newValue;
            }
        }
        catch (IOException e) {
            CommonPlugin.logError(e);
            return ""; // cannot happen, we work with string reader/writer only
        }
        finally {
            Thread.currentThread().setContextClassLoader(oldClassLoader);
        }
    }

    /**
     * Evaluates the given template, but does not create any file in the workspace;
     * in fact, non-blank output is reported via the unhandledOutput() method.
     * (The template code can still create files via FileUtils.) The method returns
     * the template processing environment, which allows the caller to access the
     * variables set by the template code.
     */
    protected Environment processTemplateForSideEffects(Configuration freemarkerConfiguration, String fileName, CreationContext context) throws CoreException {
        // classLoader stuff -- see freemarker.template.utility.ClassUtil.forName(String)
        ClassLoader oldClassLoader = Thread.currentThread().getContextClassLoader();
        Thread.currentThread().setContextClassLoader(getClassLoader());

        // substitute variables
        try {
            // perform template substitution
            Template template = freemarkerConfiguration.getTemplate(fileName, "utf8");
            StringWriter writer = new StringWriter();
            // next two lines are the expansion of: template.process(context.getVariables(), writer);
            Environment env = template.createProcessingEnvironment(context.getVariables(), writer, null);
            env.process();
            String content = writer.toString();
            if (!StringUtils.isBlank(content))
                unhandledOutput(fileName, content);
            return env;
        }
        catch (Exception e) {
            throw CommonPlugin.wrapIntoCoreException(e);
        }
        finally {
            Thread.currentThread().setContextClassLoader(oldClassLoader);
        }
    }

    /**
     * Invoked when a template that's supposed to generate no or blank output generates content.
     */
    protected void unhandledOutput(String fileName, String content) {
        throw new RuntimeException("Template " + fileName + " generated non-blank output: " + StringUtils.abbreviate(content.trim(), 30));
    }

    /**
     * Utility method for performFinish(). Copies a resource into the project,
     * performing variable substitutions in it. If the template contained
     * &lt;@setoutput path="..."&gt; tags, multiple files will be saved.
     */
    protected void createTemplatedFile(String containerRelativePath, Configuration freemarkerConfig, String templateName, CreationContext context) throws CoreException {
        assertContextFolder(context);
        createTemplatedFile(context.getFolder().getFile(new Path(containerRelativePath)), freemarkerConfig, templateName, context);
    }

    /**
     * Utility method for performFinish(). Copies a resource into the project,
     * performing variable substitutions in it. If the template contained
     * &lt;@setoutput path="..."&gt; tags, multiple files will be saved.
     */
    protected void createTemplatedFile(IFile file, Configuration freemarkerConfig, String templateName, CreationContext context) throws CoreException {
        // classLoader stuff -- see freemarker.template.utility.ClassUtil.forName(String)
        ClassLoader oldClassLoader = Thread.currentThread().getContextClassLoader();
        Thread.currentThread().setContextClassLoader(getClassLoader());

        // substitute variables
        String content;
        try {
            // perform template substitution
            Template template = freemarkerConfig.getTemplate(templateName, "utf8");
            StringWriter writer = new StringWriter();
            template.process(context.getVariables(), writer);
            content = writer.toString();
        } catch (Exception e) {
            throw CommonPlugin.wrapIntoCoreException(e);
        }
        finally {
            Thread.currentThread().setContextClassLoader(oldClassLoader);
        }

        // normalize line endings, remove multiple blank lines, etc
        content = content.replace("\r\n", "\n");
        content = content.replaceAll("\n\n\n+", "\n\n");
        content = content.trim() + "\n";

        // implement <@setoutput path="fname"/> tag: split content to files. "" means the main file
        List<String> chunks = StringUtils.splitPreservingSeparators(content, Pattern.compile(SETOUTPUT_PATTERN));
        Map<String, String> fileContent = new HashMap<String, String>(); // fileName -> content
        fileContent.put("", chunks.get(0));
        for (int i = 1; i < chunks.size(); i += 2) {
            String fileName = chunks.get(i).replaceAll(SETOUTPUT_PATTERN, "$1");
            String chunk = chunks.get(i+1);
            if (!fileContent.containsKey(fileName))
                fileContent.put(fileName, chunk);
            else
                fileContent.put(fileName, fileContent.get(fileName) + chunk);  // append
        }

        // save files (unless blank)
        for (String fileName : fileContent.keySet()) {
            String contentToSave = fileContent.get(fileName);
            if (!StringUtils.isBlank(contentToSave)) {
                // save the file if not blank. Note: we do NOT delete the existing file with
                // the same name if contentToSave is blank; this is documented behavior.
                IFile fileToSave = null;
                if (fileName.equals(""))
                    fileToSave = file; // main file
                else if (fileName.endsWith("/"))
                    fileToSave = file.getParent().getFile(new Path(fileName+file.getName())); // with main file's name, in a different directory
                else
                    fileToSave = file.getParent().getFile(new Path(fileName));
                byte[] bytes = ProjectUtils.getBytesForFile(contentToSave, file);
                createVerbatimFile(fileToSave, new ByteArrayInputStream(bytes), context);
            }
        }
    }

    /**
     * Utility method for performFinish(). Creates a file from the given input stream.
     * If the parent folder(s) do not exist, they are created. The project must exist though.
     */
    protected void createVerbatimFile(String containerRelativePath, InputStream inputStream, CreationContext context) throws CoreException {
        assertContextFolder(context);
        createVerbatimFile(context.getFolder().getFile(new Path(containerRelativePath)), inputStream, context);
    }

    /**
     * Utility method for performFinish(). Creates a file from the given input stream.
     * If the parent folder(s) do not exist, they are created. The project must exist though.
     */
    protected void createVerbatimFile(IFile file, InputStream inputStream, CreationContext context) throws CoreException {
        if (!file.getParent().exists() && file.getParent() instanceof IFolder)
            createFolder((IFolder)file.getParent(), context);
        try {
            if (!file.exists())
                file.create(inputStream, true, context.getProgressMonitor());
            else if (shouldOverwriteExistingFile(file, context))
                file.setContents(inputStream, true, true, context.getProgressMonitor());

            // check the shebang at the beginning and make it executable if it is #!
            byte buff[] = new byte[2];
            file.getContents().read(buff, 0, 2);
            boolean executable = (buff[0] == '#') && (buff[1] == '!');
            // Use reflection because this method is not supported on java 1.5 (we silently fail on java 1.5)
            Method m = ReflectionUtils.getMethod(File.class, "setExecutable", Boolean.class);
            if (m != null)
                m.invoke(file.getLocation().toFile(), executable);

        } catch (Exception e) {
            throw CommonPlugin.wrapIntoCoreException("Cannot create file: "+file.getFullPath().toString(), e);
        }
    }

    /**
     * Called back when a file already exists. Should return true if the file
     * can be overwritten (otherwise it will be skipped.)
     */
    protected boolean shouldOverwriteExistingFile(final IFile file, final CreationContext context) {
        if (Display.getCurrent() != null) {
            // we are in the UI thread -- ask.
            IWizard wizard = context.getWizard();
            Shell shell = (wizard instanceof IWizard) ? ((Wizard)wizard).getShell() : null;
            return MessageDialog.openQuestion(shell, "Confirm", "File already exists, overwrite?\n" + file.getFullPath().toString());
        }
        else {
            // we are in a background thread -- try in the UI thread
            final boolean result[] = new boolean[1];
            Display.getDefault().syncExec(new Runnable() {
                public void run() {
                    result[0] = shouldOverwriteExistingFile(file, context);
                }});
            return result[0];
        }
    }

    /**
     * Creates a folder, relative to the context resource. If the parent folder(s)
     * do not exist, they are created. The project must exist though.
     */
    protected void createFolder(String containerRelativePath, CreationContext context) throws CoreException {
        assertContextFolder(context);
        createFolder(context.getFolder().getFolder(new Path(containerRelativePath)), context);
    }

    /**
     * Creates a folder. If the parent folder(s) do not exist, they are created.
     * The project must exist though.
     */
    protected void createFolder(IFolder folder, CreationContext context) throws CoreException {
        try {
            if (!folder.getParent().exists() && folder.getParent() instanceof IFolder) // if even the project is missing, let folder.create() fail
                createFolder((IFolder)folder.getParent(), context);
            if (!folder.exists())
                folder.create(true, true, context.getProgressMonitor());
        } catch (Exception e) {
            throw CommonPlugin.wrapIntoCoreException("Cannot create folder: "+folder.getFullPath().toString(), e);
        }
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + ":" + getName();
    }
}
