package org.omnetpp.ide.installer;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLClassLoader;
import java.security.cert.Certificate;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Enumeration;
import java.util.Vector;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.jar.Manifest;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.commons.io.FileUtils;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;

/**
 * This class is responsible for installing a project into the workspace. The
 * task includes among others downloading files, extracting archives, importing
 * projects, opening projects, building projects, showing views, showing editors, etc.
 *
 * @author levy
 */
public class InstallProjectTask {
    protected URL projectDescriptionURL;
    protected ProjectInstallationOptions projectInstallationOptions;
    protected boolean verifySignature = false;
    protected byte[] certificateData = null; // TODO: DER encoded X.509 self signed certificate

    public InstallProjectTask(URL projectDescriptionURL, ProjectInstallationOptions projectInstallationOptions) {
        this.projectDescriptionURL = projectDescriptionURL;
        this.projectInstallationOptions = projectInstallationOptions;
    }

    public void run(IProgressMonitor progressMonitor) throws CoreException {
        ProjectDescription projectDescription = ProjectDescription.download(projectDescriptionURL);
        
        IProject project = ResourcesPlugin.getWorkspace().getRoot().getProject(projectDescription.getName());
        if (project != null && project.exists())
            throw new RuntimeException("Cannot install " + projectDescription.getName() + " because a project with that name already exists (rename or delete existing project to proceed)");
        AbstractProjectInstaller projectInstaller = createProjectInstaller(projectDescription, projectInstallationOptions);
        projectInstaller.run(progressMonitor);
    }

    protected AbstractProjectInstaller createProjectInstaller(ProjectDescription projectDescription, ProjectInstallationOptions projectInstallationOptions) {
        try {
            ClassLoader classLoader = getClass().getClassLoader();
            String projectInstallerURLAsString = projectDescription.getInstallerURL();
            if (projectInstallerURLAsString != null) {
                URL projectInstallerURL = new URL(projectDescriptionURL, projectInstallerURLAsString);
                File projectInstallerFile = File.createTempFile(projectDescription.getName(), "-installer.jar");
                FileUtils.copyURLToFile(projectInstallerURL, projectInstallerFile);
                if (verifySignature) {
                    CertificateFactory certificateFactory = CertificateFactory.getInstance("X.509");
                    X509Certificate certificate = (X509Certificate)certificateFactory.generateCertificate(new ByteArrayInputStream(certificateData));
                    verifySignature(new JarFile(projectInstallerFile), certificate);
                }
                classLoader = new URLClassLoader(new URL[] {projectInstallerFile.toURI().toURL()}, getClass().getClassLoader());
            }
            String installerClass = projectDescription.getInstallerClass();
            if (installerClass == null)
                installerClass = DefaultProjectInstaller.class.getCanonicalName();
            Class<?> clazz = classLoader.loadClass(installerClass);
            return (AbstractProjectInstaller)clazz.getConstructor(ProjectDescription.class, ProjectInstallationOptions.class).newInstance(projectDescription, projectInstallationOptions);
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot instantiate project installer", e);
        }
    }

    protected void verifySignature(JarFile jarFile, X509Certificate targetCert) throws IOException {
        // Sanity checking
        if (targetCert == null)
            throw new SecurityException("The certificate is invalid");

        Vector<JarEntry> entriesVec = new Vector<JarEntry>();

        // Ensure the jar file is signed.
        Manifest man = jarFile.getManifest();
        if (man == null)
            throw new SecurityException("The installer is not signed");

        // Ensure all the entries' signatures verify correctly
        byte[] buffer = new byte[8192];
        Enumeration<?> entries = jarFile.entries();

        while (entries.hasMoreElements()) {
            JarEntry je = (JarEntry) entries.nextElement();

            // Skip directories.
            if (je.isDirectory())
                continue;
            entriesVec.addElement(je);
            InputStream is = jarFile.getInputStream(je);

            // Read in each jar entry. A security exception will
            // be thrown if a signature/digest check fails.
            while (is.read(buffer, 0, buffer.length) != -1) ;
            is.close();
        }

        // Get the list of signer certificates
        Enumeration<JarEntry> e = entriesVec.elements();

        while (e.hasMoreElements()) {
            JarEntry je = e.nextElement();

            // Every file must be signed except files in META-INF.
            Certificate[] certs = je.getCertificates();
            if ((certs == null) || (certs.length == 0)) {
                if (!je.getName().startsWith("META-INF"))
                    throw new SecurityException("The installer has unsigned class files.");
            }
            else {
                // Check whether the file is signed by the expected
                // signer. The jar may be signed by multiple signers.
                // See if one of the signers is 'targetCert'.
                int startIndex = 0;
                X509Certificate[] certChain;
                boolean signedAsExpected = false;

                while ((certChain = getAChain(certs, startIndex)) != null) {
                    if (certChain[0].equals(targetCert)) {
                        // Stop since one trusted signer is found.
                        signedAsExpected = true;
                        break;
                    }
                    // Proceed to the next chain.
                    startIndex += certChain.length;
                }

                if (!signedAsExpected)
                    throw new SecurityException("The installer is not signed by a trusted signer");
            }
        }
    }

    protected X509Certificate[] getAChain(Certificate[] certs, int startIndex) {
        if (startIndex > certs.length - 1)
            return null;
        int i;
        // Keep going until the next certificate is not the
        // issuer of this certificate.
        for (i = startIndex; i < certs.length - 1; i++)
            if (!((X509Certificate)certs[i + 1]).getSubjectDN().equals(((X509Certificate)certs[i]).getIssuerDN()))
                break;
        // Construct and return the found certificate chain.
        int certChainSize = (i - startIndex) + 1;
        X509Certificate[] ret = new X509Certificate[certChainSize];
        for (int j = 0; j < certChainSize; j++)
            ret[j] = (X509Certificate)certs[startIndex + j];
        return ret;
    }
}