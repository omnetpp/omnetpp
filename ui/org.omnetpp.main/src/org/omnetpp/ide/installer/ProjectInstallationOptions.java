package org.omnetpp.ide.installer;

public class ProjectInstallationOptions {
    private String name;
    private boolean useDefaultLocation;
    private String location;

    public ProjectInstallationOptions(String name) {
        this.name = name;
        this.useDefaultLocation = true;
    }

    public ProjectInstallationOptions(String name, boolean useDefaultLocation, String location) {
        this.name = name;
        this.useDefaultLocation = useDefaultLocation;
        this.location = location;
    }

    public String getName() {
        return name;
    }

    public String getLocation() {
        return location;
    }

    public boolean getUseDefaultLocation() {
        return useDefaultLocation;
    }
}
