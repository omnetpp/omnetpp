//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "KmlUtil.h"


std::string KmlUtil::folderHeader(const char *id, const char *name, const char *description)
{
    std::stringstream out;
    out.precision(8);
    out << "<Folder";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";

    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";
    return out.str();
}

std::string KmlUtil::placemark(const char *id, float lon, float lat,float alt, const char *name, const char *description)
{
    std::stringstream out;
    out.precision(8);
    out << "<Placemark";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";
    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";
    out << "  <Style>\n";
    out << "    <IconStyle>\n";
    out << "      <Icon>\n";
    out << "        <href>http://maps.google.com/mapfiles/kml/pal4/icon28.png</href>\n";
    out << "      </Icon>\n";
    out << "    </IconStyle>\n";
    out << "  </Style>\n";
    out << "  <Point>\n";
    out << "    <altitudeMode>relativeToGround</altitudeMode>\n";
    out << "    <coordinates>" << lon << "," << lat << "," << alt << "</coordinates>\n";
    out << "  </Point>\n";
    out << "</Placemark>\n";
    return out.str();
}

std::string KmlUtil::lines(const char *id, const std::vector<KmlUtil::Pt2D>& pts, const char *name, const char *description, const char *color)
{
    std::stringstream out;
    out.precision(8);
    out << "<Placemark";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";
    if (color) {
        out << "  <Style>\n";
        out << "    <LineStyle>\n";
        out << "      <color>" << color << "</color>\n";
        out << "      <width>" << 5 << "</width>\n";
        out << "    </LineStyle>\n";
        out << "  </Style>\n";
    }
    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";
    out << "  <MultiGeometry>\n";
    for (int i=0; i<(int)pts.size()-1; i+=2)
    {
        out << "    <LineString>\n";
        out << "      <extrude>false</extrude>\n";
        out << "      <tesselate>true</tesselate>\n";
        out << "      <coordinates>";
        out << pts[i].lon << "," << pts[i].lat << " " << pts[i+1].lon << "," << pts[i+1].lat;
        out << "</coordinates>\n";
        out << "    </LineString>\n";
    }
    out << "  </MultiGeometry>\n";
    out << "</Placemark>\n";
    return out.str();
}

std::string KmlUtil::lineString(const char *id, const std::vector<KmlUtil::Pt2D>& pts, const char *name, const char *description, const char *color)
{
    std::stringstream out;
    out.precision(8);
    out << "<Placemark";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";
    if (color) {
        out << "  <Style>\n";
        out << "    <LineStyle>\n";
        out << "      <color>" << color << "</color>\n";
        out << "    </LineStyle>\n";
        out << "  </Style>\n";
    }
    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";
    out << "  <LineString>\n";
    out << "    <extrude>false</extrude>\n";
    out << "    <tesselate>true</tesselate>\n";
    out << "    <coordinates>";
    for (int i=0; i<(int)pts.size(); i++)
        out << pts[i].lon << "," << pts[i].lat << " ";
    out << "</coordinates>\n";
    out << "  </LineString>\n";
    out << "</Placemark>\n";
    return out.str();
}

std::string KmlUtil::disk(const char *id, float lon, float lat, float r, const char *name, const char *description, const char *color)
{
    static const int N = 40;
    static float x[N], y[N];
    static bool initialized = false;
    if (!initialized) {
        for (int i=0; i<N; i++) {
            x[i] = cos(i*M_PI*2/N);
            y[i] = sin(i*M_PI*2/N);
        }
        initialized = true;
    }

    std::stringstream out;
    out.precision(8);
    out << "<Placemark";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";
    if (color) {
        out << "  <Style>\n";
        out << "    <LineStyle>\n";
        out << "      <width>0</width>\n";
        out << "    </LineStyle>\n";
        out << "    <PolyStyle>\n";
        out << "      <color>" << color << "</color>\n";
        out << "    </PolyStyle>\n";
        out << "  </Style>\n";
    }
    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";
    out << "  <Polygon>\n";
    out << "    <extrude>false</extrude>\n";
    out << "    <outerBoundaryIs>\n";
    out << "      <LinearRing>\n";
    out << "        <coordinates>\n";
    for (int i=0; i<N; i++) {
        double lati = y2lat(lat, y[i]*r);
        double loni = x2lon(lati, lon, x[i]*r);
        out << loni << "," << lati << " ";
    }
    out << "        </coordinates>\n";
    out << "      </LinearRing>\n";
    out << "    </outerBoundaryIs>\n";
    out << "  </Polygon>\n";
    out << "</Placemark>\n";
    return out.str();
}

std::string KmlUtil::model(const char *id, float lon, float lat, float heading, float scale, const char *link, const char *name, const char *description)
{
    std::stringstream out;
    out.precision(8);
    out << "<Placemark";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";
    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";

    out << "  <Model>\n";
    out << "    <altitudeMode>relativeToGround</altitudeMode>\n";
    out << "    <Location>\n";
    out << "      <longitude>" << lon << "</longitude>\n";
    out << "      <latitude>" << lat << "</latitude>\n";
    //out << "      <altitude>0</altitude>\n";
    out << "    </Location>\n";
    out << "    <Orientation>\n";
    out << "      <heading>" << heading << "</heading>\n";
    //out << "      <tilt>0</tilt>\n";
    //out << "      <roll>0</roll>\n";
    out << "    </Orientation>\n";
    out << "    <Scale>\n";
    out << "      <x>" << scale << "</x>\n";
    out << "      <y>" << scale << "</y>\n";
    out << "      <z>" << scale << "</z>\n";
    out << "    </Scale>\n";
    out << "    <Link>\n";
    out << "      <href>" << link << "</href>\n";
    out << "    </Link>\n";
    out << "  </Model>\n";

    out << "</Placemark>\n";
    return out.str();
}

std::string KmlUtil::track(const char *id, const std::vector<Pt2D>& pts, float timeStep, float modelScale, const char *modelLink, const char *name, const char *description, const char *color)
{
    std::stringstream out;
    out.precision(8);

    out << "<Placemark";
    if (id)
        out << " id='" << id << "' ";
    out << ">\n";
    if (color) {
        out << "  <Style>\n";
        out << "    <LineStyle>\n";
        out << "      <color>" << color << "</color>\n";
        out << "      <width>5</width>\n";
        out << "    </LineStyle>\n";
        out << "  </Style>\n";
    }
    if (name)
        out << "  <name>" << name << "</name>\n";
    if (description)
        out << "  <description>" << description << "</description>\n";

    out << "  <gx:Track>\n";
    double t = SIMTIME_DBL(simTime() - (pts.size()-1)*timeStep);
    for (int i=0; i<(int)pts.size(); i++, t+= timeStep) {
        int x = (int)t; //NOTE: timeStep < 1 will result in multiple timestamps with the same value!!!
        int ss = x % 60; x-= ss; x/=60;
        int mm = x % 60; x-= mm; x/=60;
        int hh = x;
        char when[100];
        sprintf(when, "2010-05-28T%2.2d:%2.2d:%2.2dZ", hh, mm, ss); //TODO better conversion! (localtime()+sprintf()?)
        out << "    <when>" << when << "</when>\n";
    }
    for (int i=0; i<(int)pts.size(); i++)
        out << "    <gx:coord>" << pts[i].lon << " " << pts[i].lat << "</gx:coord>\n";

    if (modelLink) {
        out << "    <Model>\n";
        out << "    <Orientation>\n";
        out << "      <heading>" << 0 << "</heading>\n"; //TODO compute from first two points
        out << "    </Orientation>\n";
        out << "    <Scale>\n";
        out << "      <x>" << modelScale << "</x>\n";
        out << "      <y>" << modelScale << "</y>\n";
        out << "      <z>" << modelScale << "</z>\n";
        out << "    </Scale>\n";
        out << "    <Link>\n";
        out << "      <href>" << modelLink << "</href>\n";
        out << "    </Link>\n";
        out << "  </Model>\n";
    }
    out << "  </gx:Track>\n";

    out << "</Placemark>\n";

    return out.str();
}


void KmlUtil::hsbToRgb(double hue, double saturation, double brightness,
                       double& red, double& green, double &blue)
{
   if (brightness == 0.0)
   {   // safety short circuit again
       red   = 0.0;
       green = 0.0;
       blue  = 0.0;
       return;
   }

   if (saturation == 0.0)
   {   // grey
       red   = brightness;
       green = brightness;
       blue  = brightness;
       return;
   }

   float domainOffset;         // hue mod 1/6
   if (hue < 1.0/6)
   {   // red domain; green ascends
       domainOffset = hue;
       red   = brightness;
       blue  = brightness * (1.0 - saturation);
       green = blue + (brightness - blue) * domainOffset * 6;
   }
     else if (hue < 2.0/6)
     { // yellow domain; red descends
       domainOffset = hue - 1.0/6;
       green = brightness;
       blue  = brightness * (1.0 - saturation);
       red   = green - (brightness - blue) * domainOffset * 6;
     }
     else if (hue < 3.0/6)
     { // green domain; blue ascends
       domainOffset = hue - 2.0/6;
       green = brightness;
       red   = brightness * (1.0 - saturation);
       blue  = red + (brightness - red) * domainOffset * 6;
     }
     else if (hue < 4.0/6)
     { // cyan domain; green descends
       domainOffset = hue - 3.0/6;
       blue  = brightness;
       red   = brightness * (1.0 - saturation);
       green = blue - (brightness - red) * domainOffset * 6;
     }
     else if (hue < 5.0/6)
     { // blue domain; red ascends
       domainOffset = hue - 4.0/6;
       blue  = brightness;
       green = brightness * (1.0 - saturation);
       red   = green + (brightness - green) * domainOffset * 6;
     }
     else
     { // magenta domain; blue descends
       domainOffset = hue - 5.0/6;
       red   = brightness;
       green = brightness * (1.0 - saturation);
       blue  = red - (brightness - green) * domainOffset * 6;
     }
}

