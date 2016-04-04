#!/usr/bin/env python
#
# opp_featuretool: This script manipulates omnetpp project features.
#
# Copyright (C) 2016 OpenSim Ltd.
# Author: Zoltan Bojthe
#

from __future__ import print_function
import argparse
import csv
import os
import sys
import re
import types
import xml.dom.minidom

FEATURESFILE = ".oppfeatures"
FEATURESTATEFILE = ".oppfeaturestate"
NEDFOLDERSFILE = ".nedfolders"
NEDEXCLUSIONSFILE = ".nedexclusions"

def fail(msg):
    print("opp_featuretool: Error: {}".format(msg), file=sys.stderr)
    sys.exit(1)

def warn(msg):
    print("opp_featuretool: Warning: {}".format(msg), file=sys.stderr)

def queryYesNo(question, default="yes"):
    """Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is True for "yes" or False for "no".
    """
    valid = {"yes": True, "y": True, "ye": True,
             "no": False, "n": False}
    if default is None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "
                             "(or 'y' or 'n').\n")

############################
class Feature:
    def __init__(self, feature, ord):
        self.id = feature.getAttribute("id")
        self.name = feature.getAttribute("name")
        self.description = feature.getAttribute("description")
        self.initiallyEnabled = True
        if feature.hasAttribute("initiallyEnabled"):
            self.initiallyEnabled = feature.getAttribute("initiallyEnabled") == 'true'
        self.requires = feature.getAttribute("requires").split()
        self.labels = feature.getAttribute("labels").split()
        self.nedPackages = feature.getAttribute("nedPackages").split()
        self.extraSourceFolders = feature.getAttribute("extraSourceFolders").split()
        self.compileFlags = feature.getAttribute("compileFlags").split()
        self.linkerFlags = feature.getAttribute("linkerFlags").split()
        self.ord = ord

############################
class FeatureState:
    def __init__(self, id, enabled, ord):
        self.id = id
        self.enabled = enabled
        self.ord = ord

    @classmethod
    def fromXML(cls, xmlelement, ord):
        return cls(xmlelement.getAttribute("id"), xmlelement.getAttribute("enabled").lower() == 'true', ord)

    def __repr__(self):
        return "<%s: id=%s, enabled=%s, ord=%d)" % (self.__class__.__name__, self.id, self.enabled, self.ord)

############################
class NedFolder:
    def __init__(self, name, path, prefix, ord):
        self.name = name
        self.path = path
        self.prefix = prefix
        self.ord = ord

    def isSubpackage(self, subpkg):
        if self.prefix == '':
             return True
        return subpkg.startswith(self.prefix) and (len(subpkg) == len(self.prefix) or subpkg[len(self.prefix)] == '.')

    def __repr__(self):
        return "<%s: name=%s, path=%s, prefix=%s, ord=%d>" % (self.__class__.__name__, self.name, self.path, self.prefix, self.ord)

############################
class FeatureTool:
    def __init__(self):
        self.fixingMode = False
        self.autoFixingMode = False
        self.errorOccurred = False
        self.fsChanged = False
        self.nedExclusionFileChanged = False
        self.nedfolders = []
        self.nedfoldersExcluded = []
        self.cppSourceRoots = []

    def printIfVerbose(self, verbosity, msg):
        if verbosity <= self.args.verbose:
            print(msg)

    def warnIfVerbose(self, verbosity, msg):
        if verbosity <= self.args.verbose:
            print("opp_featuretool: Warning: {}".format(msg), file=sys.stderr)

    def createParser(self):
        self.parser = argparse.ArgumentParser(description='''Turn project features on and off in an OMNeT++/OMNEST model. List the enablement
of the features. Show the command line options needed to generarate a makefile
when using the opp_makemake command. The tool must be executed in the project
root directory where the following files are present:
.oppfeatures, .oppfeaturestatus, .nedfolders, .nedexclusions''',
                formatter_class=argparse.RawDescriptionHelpFormatter)
        subparsers = self.parser.add_subparsers(help='', dest='command', metavar='COMMAND')

        # list command
        list_parser = subparsers.add_parser('list', help='List features')
        list_group = list_parser.add_mutually_exclusive_group()
        list_group.add_argument('-a', '--all', action='store_true',
                    default=False,
                    help='List all features (default)',
                    )
        list_group.add_argument('-e', '--enabled', action='store_true',
                    default=False,
                    help='List enabled features',
                    )
        list_group.add_argument('-d', '--disabled', action='store_true',
                    default=False,
                    help='List disabled features',
                    )
        list_group.add_argument('-x', '--diff', action='store_true',
                    default=False,
                    help='List features not in default state',
                    )

        # validate command
        validate_parser = subparsers.add_parser('validate', help='Validate feature enablements: Report inconsistencies and dependency problems in the '+
                FEATURESTATEFILE+' and '+NEDEXCLUSIONSFILE+' file.')

        # reset command
        reset_parser = subparsers.add_parser('reset', help='Reset all feature enablements to their default')

        # repair command
        repair_parser = subparsers.add_parser('repair', help='Repair feature enablements: missing features are added to the state file with the initiallyEnabled flag, non-existent features are removed from the '+FEATURESTATEFILE+' file')

        # enable command
        enable_parser = subparsers.add_parser('enable', help='Enable the specified features')
        enable_parser.add_argument('features', nargs='+',
                    help='Enable the specified features, use \'all\' for all features',
                    )
        enable_parser.add_argument('-f', '--with-dependencies', action='store_true',
                    default=False,
                    help='Enable all required features without asking for confirmation',
                    )

        # disable command
        disable_parser = subparsers.add_parser('disable', help='Disable the specified features')
        disable_parser.add_argument('features', nargs='+',
                    help='Disable the specified features, use \'all\' for all features',
                    )
        disable_parser.add_argument('-f', '--with-dependencies', action='store_true',
                    default=False,
                    help='Disable all dependent features without asking for confirmation',
                    )

        # options command:
        options_parser = subparsers.add_parser('options', help='Print opp_makemake command line arguments for creating a make file with the current feature enablement')
        options_parser.add_argument('-s', '--srcpath',
                    help='Selects the source folder to print makemake options for. (The default is the first value in the cppSourceRoots attribute of the <features> tag in the FEATUREFILE.)',
                    )
        options_parser.add_argument('-c', '--compiler-options', action='store_true', help='show compiler options (i.e. -D flags)')
        options_parser.add_argument('-l', '--linker-options', action='store_true', help='show linker options (i.e. -l and -L flags)')
        options_parser.add_argument('-f', '--folder-options', action='store_true', help='show excluded folders options (i.e. -X flags)')

        # defines command:
        defines_parser = subparsers.add_parser('defines', help='Print the compiler macros that can be used as a header file (i.e. the -DWITH_FEATURE options from the project features)')

        # isenabled command
        isenabled_parser = subparsers.add_parser('isenabled', help='Returns true if the specified feature(s) are enabled, and false otherwise.')
        isenabled_parser.add_argument('features', nargs='+',
                    help='feature list',
                    )

        # common arguments:
        self.parser.add_argument('-v', '--verbose', action='count', default=1, help='Verbose mode')
        self.parser.add_argument('-q', '--quiet', action='store_const', const=0, dest='verbose', help='Quiet mode')

    ############################
    def doResetCommand(self):
        # RESET command
        self.printIfVerbose(1, "Enablement of all project features reset to the default")
        self.featurestate = dict()
        for fk, feature in self.sortedFeatures:
            fs = FeatureState(feature.id, feature.initiallyEnabled, feature.ord)
            self.featurestate[fk] = fs
        self.writeFeatureState()

        self.nedfoldersExcluded = []
        for fk, feature in self.sortedFeatures:
            if not feature.initiallyEnabled:
                self.nedfoldersExcluded.extend(feature.nedPackages)

        self.writeNedExclusions()


    ############################
    def readFeatures(self):
        # read features xml file
        try:
            featurefile = open(FEATURESFILE, 'r')
        except IOError as e:
            fail("can't open {} file: {}".format(FEATURESFILE, e))

        try:
            DOMTree = xml.dom.minidom.parse(featurefile)
            featuresDom = DOMTree.documentElement
            self.cppSourceRoots = featuresDom.getAttribute("cppSourceRoots").split()
            featurelistDom = featuresDom.getElementsByTagName("feature")

            self.features = dict()
            ord = 0
            for featureDom in featurelistDom:
                feature = Feature(featureDom, ord)
                self.features[feature.id] = feature
                ord += 1
        except Exception as e:
            fail("cannot parse {} file: {}".format(FEATURESFILE, e))

        self.sortedFeatures = sorted(self.features.items(), key=lambda x:x[1].ord)
        featurefile.close()


    ############################
    def readNedFoldersFile(self):
        # read nedfolders file:
        self.nedfolders = []

        emptyline = re.compile(r'^\s*$')
        check = re.compile(r'^\-?([a-zA-Z0-9_]+\.)*[a-zA-Z0-9_]+$')

        if not os.path.isfile(NEDFOLDERSFILE):
            # warn("the "+NEDFOLDERSFILE+" file is missing.")
            nedfilename = './package.ned'
            prefix = self.getPrefixFromPackageNedFile(nedfilename)
            self.nedfolders.append(NedFolder('.', '.', prefix, 0))
            return

        try:
            nedfoldersfile = open(NEDFOLDERSFILE, 'r')
        except IOError as e:
            fail("can't open {} file: {}".format(NEDFOLDERSFILE, e))

        try:
            ord = 0
            for line in nedfoldersfile:
                ord += 1
                line = line.rstrip('\n')
                if emptyline.match(line):
                    continue
                if not check.match(line):
                    fail("invalid line at %s:%d : '%s'" % (NEDFOLDERSFILE, ord, line))
                elif line[0] == '-':
                    pass    # ignore omnet 4.x exclusion lines
                else:
                    if line == '.':
                        path = line
                    else:
                        path = line.replace('.', '/')
                    prefix = ''
                    nedfilename = path+'/package.ned'
                    prefix = self.getPrefixFromPackageNedFile(nedfilename)
                    self.nedfolders.append(NedFolder(line, path, prefix, ord))
            nedfoldersfile.close()
        except IOError as e:
            fail("error occurred when reading the {} file: {}".format(NEDFOLDERSFILE, e))

    ############################
    def getPrefixFromPackageNedFile(self, nedfilename):
        pkgchk = re.compile(r'^package\s+(([a-zA-Z0-9_]+\.)*[a-zA-Z0-9_]+);')

        if os.path.isfile(nedfilename):
            try:
                with open(nedfilename, 'r') as nedfile:
                    for nedline in nedfile:
                        m = pkgchk.match(nedline)
                        if m:
                            prefix = m.group(1)
                            return prefix
            except IOError as e:
                fail("error reading {}: {}".format(nedfilename, e))
        return ''

    ############################
    def readNedExclusionsFile(self):
        # read nedexclusions file:
        self.nedfoldersExcluded = []

        if not os.path.isfile(NEDEXCLUSIONSFILE):
            # warn("the "+NEDEXCLUSIONSFILE+" file is missing.")
            return

        try:
            nedexclusionsfile = open(NEDEXCLUSIONSFILE, 'r')
        except IOError as e:
            fail(".nedexclusions file error: {}".format(e))

        check = re.compile(r'^([a-zA-Z0-9_]+\.)*[a-zA-Z0-9_]+$')
        try:
            ord = 0
            for line in nedexclusionsfile:
                ord += 1
                line = line.rstrip('\n')
                if not check.match(line):
                    if self.fixingMode:
                        warn("invalid line in %s:%d, removed: '%s'" % (NEDEXCLUSIONSFILE, ord, line))
                        self.nedExclusionFileChanged = True
                    elif self.autoFixingMode:
                        warn("invalid line in %s:%d, ignored: '%s'" % (NEDEXCLUSIONSFILE, ord, line))
                    else:
                        fail("invalid line in %s:%d, '%s'" % (NEDEXCLUSIONSFILE, ord, line))
                else:
                    self.nedfoldersExcluded.append(line)
            nedexclusionsfile.close()
        except IOError as e:
            fail("I/O error while reading the {} file: ({})".format(NEDEXCLUSIONSFILE, e))


    ############################
    def writeNedExclusions(self):
        try:
            nedexclusionsfile = open(NEDEXCLUSIONSFILE, 'w')
            for nf in sorted(self.nedfoldersExcluded):
                nedexclusionsfile.write("%s\n" % (nf))
            nedexclusionsfile.close()
        except IOError as e:
            fail("I/O error while writing the {} file: ({})".format(NEDEXCLUSIONSFILE, e))


    ############################
    def writeFeatureState(self):
        try:
            DOMTree = xml.dom.minidom.parseString("<featurestates/>")
            featurestateDom = DOMTree.documentElement
            for fk, fs in sorted(self.featurestate.items(), key=lambda x:x[1].ord):
                oneFS = DOMTree.createElement("feature")
                oneFS.setAttribute("id", fs.id)
                oneFS.setAttribute("enabled", str(fs.enabled).lower())
                featurestateDom.appendChild(oneFS)
            fsFile = open(FEATURESTATEFILE, 'w')
            DOMTree.writexml(fsFile, addindent="    ", newl="\n")
            fsFile.close()
        except IOError as e:
            fail("error occurred when writing {} file: {}".format(FEATURESTATEFILE, e))


    ############################
    def readFeatureState(self):
        # read featurestate xml file
        self.featurestate = dict()

        if not os.path.isfile(FEATURESTATEFILE):
            if (self.fixingMode or self.autoFixingMode):
                # warn("the .featurestate file does not exist. Using defaults.")
                # generate default featurestate
                for fk, feature in self.sortedFeatures:
                    fs = FeatureState(feature.id, feature.initiallyEnabled, feature.ord)
                    self.featurestate[fk] = fs

                if self.fixingMode:
                    self.fsChanged = True
            else:
                fail("the "+FEATURESTATEFILE+" file does not exist.")
        else:
            try:
                fsFile = open(FEATURESTATEFILE, 'r')
            except IOError as e:
                fail("I/O error while reading the {} file: {}".format(FEATURESTATEFILE, e))

            if os.stat(FEATURESTATEFILE).st_size == 0:
                return
            try:
                DOMTree = xml.dom.minidom.parse(fsFile)
            except Exception as e:
                fail("cannot parse {} file: {}, to fix it: repair the file by hand, or delete {}".format(FEATURESTATEFILE, e, FEATURESTATEFILE))

            fsFile.close()

            featurestateDom = DOMTree.documentElement
            featurestatelistDom = featurestateDom.getElementsByTagName("feature")

            xord = len(self.features) + 1000
            for featureDom in featurestatelistDom:
                featureState = FeatureState.fromXML(featureDom, xord)
                if featureState.id in self.features:
                    featureState.ord = self.features[featureState.id].ord
                elif self.fixingMode:
                    warn(""+FEATURESTATEFILE+": Line '" + featureDom.toxml() + "' contains the unknown feature '" + featureState.id + "', removed")
                    self.fsChanged = True
                    xord += 1
                    continue
                else:
                    fail(""+FEATURESTATEFILE+": Line '" + featureDom.toxml() + "' contains the unknown feature '" + featureState.id + "'")

                if featureState.id in self.featurestate:
                    if self.fixingMode:
                        warn(""+FEATURESTATEFILE+": Line '" + featureDom.toxml() + "' contains the same feature more than one time '" + featureState.id + "', removed")
                        self.fsChanged = True
                        continue
                    else:
                        fail(""+FEATURESTATEFILE+": Line '" + featureDom.toxml() + "' contains the same feature more than one time '" + featureState.id + "'")
                else:
                    self.featurestate[featureState.id] = featureState


    ############################
    def isCxxSourceFolder(self, folder):
        if len(self.cppSourceRoots) == 0:
            return True
        for cppSourceRoot in self.cppSourceRoots:
            if folder.startswith(cppSourceRoot) and (len(folder) == len(cppSourceRoot) or folder[len(cppSourceRoot)] == '/'):
                return True;
        return False;


    ############################
    def getNedBasedCxxSourceFolders(self, feature):
        result = []
        for nedPackage in feature.nedPackages:
            for nedfolder in self.nedfolders:
                if nedfolder.isSubpackage(nedPackage):
                    packageSuffix = nedPackage[len(nedfolder.prefix):]
                    folder = nedfolder.path + '/' + packageSuffix.replace('.', '/')
                    if (os.path.exists(folder) and self.isCxxSourceFolder(folder)):
                        result.append(folder)
        return result


    ############################
    def checkFeatureNedFolders(self, feature):
        retval = True
        for nedPackage in feature.nedPackages:
            foundNedPackageFolder = False
            for nedfolder in self.nedfolders:
                if nedfolder.isSubpackage(nedPackage):
                    packageSuffix = nedPackage[len(nedfolder.prefix):]
                    folder = nedfolder.path + '/' + packageSuffix.replace('.', '/')
                    if os.path.exists(folder):
                        foundNedPackageFolder = True
            if not foundNedPackageFolder:
                print("opp_featuretool: Error: NED package '{}' in feature '{}' was not found.".format(nedPackage, feature.id), file=sys.stderr)
                retval = False
        return retval


    ############################
    def verifyFeaturesNedFolders(self):
        ok = True
        for fid, feature in self.sortedFeatures:
            if not self.checkFeatureNedFolders(feature):
                ok = False
        if not ok:
            fail("Check whether all NED folders are set properly (in the {} file) and all directories corresponding to the NED packages defined in the {} file do exist.".format(NEDFOLDERSFILE, FEATURESFILE))


    ############################
    def doListCommand(self):
        self.doValidateCommand()
        # LIST command #
        if self.args.enabled:
            categ = "enabled "
            self.printIfVerbose(2, "List of enabled features:")
        elif self.args.disabled:
            categ = "disabled "
            self.printIfVerbose(2, "List of disabled features:")
        elif self.args.diff:
            categ = "changed "
            self.printIfVerbose(2, "List of changed features:")
        else:
            categ = ""
            self.printIfVerbose(2, "List of all features:")
            self.args.all = True

        cnt = 0
        for key, fs in sorted(self.featurestate.items(), key=lambda x:x[1].ord):
            if not fs.id in self.features:
                fail("unknown %s '%s' feature (not found in .oppfeature file)" % ('enabled' if fs.enabled else 'disabled', fs.id))
            elif self.args.all or (self.args.enabled and fs.enabled) or (self.args.disabled and not fs.enabled) or (self.args.diff and fs.enabled != self.features[fs.id].initiallyEnabled):
                print("    %s %s" % ('+' if fs.enabled else '-', fs.id))
                cnt += 1
        for key, fi in self.sortedFeatures:
            if not key in self.featurestate:
                fail("feature '{}' is missing from the {} file".format(key, FEATURESTATEFILE))
        self.printIfVerbose(2, "{} {}feature(s) found.".format(cnt, categ))


    ############################
    def updateRequirementsOf(self, featureid, featureOnList, requirements):
        for req in self.features[featureid].requires:
            if not self.featurestate[req].enabled:
                if not req in requirements and not req in featureOnList:
                    self.updateRequirementsOf(req, featureOnList, requirements)
                requirements.add(req)
    ############################
    def updateRequirements(self, featureOnList, requirements):
        for featureid in featureOnList:
            self.updateRequirementsOf(featureid, featureOnList, requirements)
    ############################
    def doEnableCommand(self):
        self.doValidateCommand()
        # ENABLE command #
        if 'all' in self.args.features:
            if len(self.args.features) > 1:
                fail("'all' should not be used while individual features are mentioned on the command line.")
            else:
                for key,fs in self.sortedFeatures:
                    if not self.featurestate[key].enabled:
                        self.fsChanged = True
                        self.featurestate[key].enabled = True
        else:
            featureOnList = set()
            requirements = set()
            for key in self.args.features:
                if not key in self.features:
                    fail("unknown feature '%s'." % (key))
                if not self.featurestate[key].enabled:
                    featureOnList.add(key)
            self.updateRequirements(featureOnList, requirements)
            update = True
            if len(featureOnList) == 0:
                self.printIfVerbose(1, "Feature(s) are already enabled.")
                sys.exit(0)
            self.printIfVerbose(1, "Enabling feature(s):  " + ", ".join(featureOnList))
            if len(requirements):
                if self.args.with_dependencies:
                    self.printIfVerbose(1, "Required features that are enabled, too:  " + ", ".join(sorted(requirements)))
                else:
                    print("Required features:  " + ", ".join(sorted(requirements)))
                    update = queryYesNo("Enable these features?", default="yes")
            if update:
                featureOnList.update(requirements)
                for key in featureOnList:
                    self.featurestate[key].enabled = True
                    self.fsChanged = True


    ############################
    def updateDependsOf(self, featureid, featureOffList, dependencyList):
        for fk,f in self.sortedFeatures:
            if (featureid in f.requires) and self.featurestate[f.id].enabled and (not (f.id in dependencyList)) and (not (f.id in featureOffList)):
                self.updateDependsOf(f.id, featureOffList, dependencyList)
                dependencyList.add(f.id)
    ############################
    def updateDependencies(self, featureOffList, dependencyList):
        for featureid in featureOffList:
            self.updateDependsOf(featureid, featureOffList, dependencyList)
    ############################
    def doDisableCommand(self):
        self.doValidateCommand()
        # DISABLE command #
        if 'all' in self.args.features:
            if len(self.args.features) > 1:
                fail("'all' should not be used while individual features are mentioned on the command line.")
            else:
                for key,fs in self.sortedFeatures:
                    if self.featurestate[key].enabled:
                        self.featurestate[key].enabled = False
                        self.fsChanged = True
        else:
            featureOffList = set()
            dependencyList = set()
            for key in self.args.features:
                if not key in self.features:
                    fail("unknown feature '%s'." % (key))
                if self.featurestate[key].enabled:
                    featureOffList.add(key)
            self.updateDependencies(featureOffList, dependencyList)
            update = True
            if len(featureOffList) == 0:
                self.printIfVerbose(1, "All the selected features are already disabled.")
                sys.exit(0)
            self.printIfVerbose(1, "Disabling feature(s):  " + ", ".join(featureOffList))
            if len(dependencyList):
                if self.args.with_dependencies:
                    self.printIfVerbose(1, "Dependent features that are disabled, too:  " + ", ".join(sorted(dependencyList)))
                else:
                    print("Dependent features:  " + ", ".join(sorted(dependencyList)))
                    update = queryYesNo("Disable these features, too?", default="yes")
            if update:
                featureOffList.update(dependencyList)
                for key in featureOffList:
                    self.featurestate[key].enabled = False
                    self.fsChanged = True


    ############################
    def doRepairCommand(self):
        # "creation of default FEATURESTATEFILE file when it missing" for 'prepare' implemented in readFeatureState()
        self.doValidateCommand()


    ############################
    def doValidateCommand(self):
        # "syntax check in FEATURESTATEFILE file" for 'validate' implemented in readFeatureState()

        featureMissed = False
        # check feature existing in FEATURESTATEFILE file:
        for fid, feature in self.sortedFeatures:
            if not fid in self.featurestate:
                featureMissed = True
                fs = FeatureState(feature.id, feature.initiallyEnabled, feature.ord)
                self.featurestate[fid] = fs
                if self.fixingMode:
                    warn("feature '%s' is missing from the %s file, adding it with default state." % (fid, FEATURESTATEFILE))
                    self.fsChanged = True
                elif self.autoFixingMode:
                    warn("feature '%s' is missing from the %s file, using the default state." % (fid, FEATURESTATEFILE))
                else:
                    fail("feature '%s' is missing from the %s file." % (fid, FEATURESTATEFILE))

        # check dependencies:
        dependencyErrorOccurred = False
        featureOnList = set()
        requirements = set()
        for fid, feature in self.sortedFeatures:
            if self.featurestate[fid].enabled:
                for r in feature.requires:
                    if not self.featurestate[r].enabled:
                        warn("feature '%s' is required for '%s', but it is disabled." % (r, fid))
                        featureOnList.add(r)
                        dependencyErrorOccurred = True
        if len(featureOnList):
            self.updateRequirements(featureOnList, requirements)
            if len(requirements):
                warn("these features are also required:  " + ", ".join(requirements))
            featureOnList.update(requirements)

        nedfolderErrorOccurred = False
        for fid, feature in self.sortedFeatures:
            for np in feature.nedPackages:
                if self.featurestate[fid].enabled:
                    if np in self.nedfoldersExcluded:
                        if not (self.autoFixingMode or self.fixingMode):
                            warn("NED package '%s' is part of the enabled feature '%s', but it is excluded." % (np, fid))
                        nedfolderErrorOccurred = True
                else:
                    if not np in self.nedfoldersExcluded:
                        if not (self.autoFixingMode or self.fixingMode):
                            warn("NED package '%s' is part of the disabled feature '%s', but it is not excluded." % (np, fid))
                        nedfolderErrorOccurred = True
        if dependencyErrorOccurred:
            if self.fixingMode:
                for key in featureOnList:
                    self.featurestate[key].enabled = True
                self.fsChanged = True
            else:
                fail("feature dependency error(s) occurred.")  #FIXME what occurred???
        if nedfolderErrorOccurred:
            if self.fixingMode:
                self.fsChanged = True
            elif not self.autoFixingMode:
                fail("feature dependency error(s) found in the "+NEDEXCLUSIONSFILE+" file.")  #FIXME what kind of errors???


    ############################
    def doOptionsCommand(self):
        self.doValidateCommand()
        if self.args.srcpath == None:
            self.args.srcpath = self.cppSourceRoots[0]
        elif self.args.srcpath not in self.cppSourceRoots:
            fail("the selected '%s' source path is not specified in the '%s' file. Choose any of ['%s']." % (self.args.srcpath, FEATURESFILE, "','".join(self.cppSourceRoots)))
        if (not (self.args.compiler_options or self.args.folder_options or self.args.linker_options)):
            # enable all
            self.args.compiler_options = True
            self.args.folder_options = True
            self.args.linker_options = True
        extraSourceFolders = []
        excludedExtraSourceFolders = []
        excludedSourceFolders = []
        compileFlags = []
        linkerFlags = []
        for fid,feature in self.sortedFeatures:
            if self.featurestate[fid].enabled:
                extraSourceFolders.extend(feature.extraSourceFolders)
                compileFlags.extend(feature.compileFlags)
                linkerFlags.extend(feature.linkerFlags)
            else:
                excludedExtraSourceFolders.extend(feature.extraSourceFolders)
                excludedSourceFolders.extend(self.getNedBasedCxxSourceFolders(feature))

        extraSourceFolders = ["-d"+x for x in extraSourceFolders]

        excludedSrcFolders = []
        s = self.args.srcpath+'/'
        for f in excludedSourceFolders + excludedExtraSourceFolders:
            if f.startswith(s):
                excludedSrcFolders.append("-X"+f[len(s):])

        flags = []
#        if self.args.????_options:
#            flags.extend(extraSourceFolders)
        if self.args.folder_options:
            flags.extend(excludedSrcFolders)
        if self.args.compiler_options:
            flags.extend(compileFlags)
        if self.args.linker_options:
            flags.extend(linkerFlags)

        print(" ".join(flags))


    ############################
    def doDefinesCommand(self):
        self.doValidateCommand()
        compileFlags = []
        for fid,feature in self.sortedFeatures:
            if self.featurestate[fid].enabled:
                compileFlags.extend(feature.compileFlags)

        defline = re.compile(r'^-D([a-zA-Z0-9_]+)$')
        deflineeq = re.compile(r'^-D([a-zA-Z0-9_]+)=(.*)$')
        flags = " ".join(compileFlags).split(" ");
        for flag in flags:
            matchObj = defline.match(flag)
            if matchObj:
                print("#ifndef {}\n#define {}\n#endif\n".format(matchObj.group(1), matchObj.group(1)))
                continue
            matchObj = deflineeq.match(flag)
            if matchObj:
                print("#ifndef {}\n#define {} = {}#endif\n".format(matchObj.group(1), matchObj.group(1), matchObj.group(2)))
                continue


    ############################
    def doIsEnabledCommand(self):
        self.doValidateCommand()
        # ISENABLED command #
        featureOnList = set()
        featureUnknownList = set()
        for key in self.args.features:
            if not key in self.features:
                featureUnknownList.add(key)
            elif not self.featurestate[key].enabled:
                featureOnList.add(key)
        if len(featureUnknownList) > 0:
            fail("Unknown feature(s): {}.".format((", ".join(sorted(featureUnknownList)))))
        if len(featureOnList) > 0:
            if self.args.verbose >= 2:
                print("Disabled feature(s): {}.".format((", ".join(sorted(featureOnList)))), file=sys.stderr)
            sys.exit(1)
        self.printIfVerbose(1, "Feature(s) {} are enabled.".format((", ".join(self.args.features))))
        sys.exit(0)


    ############################
    def run(self):
        self.errorOccurred = False
        self.fsChanged = False
        self.features = dict()

        self.createParser()
        try:
            self.args = self.parser.parse_args()
        except IOError as e:
            fail("{}".format(e))

        self.autoFixingMode = (self.args.command != "validate")
        self.fixingMode = (self.args.command == "repair")

        # read nedfolders file:
        self.readNedFoldersFile()

        # read feature file
        self.readFeatures()

        self.verifyFeaturesNedFolders()

        if self.args.command == 'reset':
            self.doResetCommand()
            return

        # Read featurestate file #
        self.readFeatureState()

        # read nedexclusions file:
        self.readNedExclusionsFile()

        if self.args.command == 'list':
            self.doListCommand()
        elif self.args.command == 'validate':
            self.doValidateCommand()
        elif self.args.command == 'repair':
            self.doRepairCommand()
        elif self.args.command == 'reset':
            fail("reset command already processed")  #FIXME assert(false) # reset command already processed
        elif self.args.command == 'enable':
            self.doEnableCommand()
        elif self.args.command == 'disable':
            self.doDisableCommand()
        elif self.args.command == 'options':
            self.doOptionsCommand()
        elif self.args.command == 'defines':
            self.doDefinesCommand()
        elif self.args.command == 'isenabled':
            self.doIsEnabledCommand()
        else:
            fail("unknown command '%s'" % self.args.command)

        if self.fsChanged:
            self.writeFeatureState()
            self.printIfVerbose(1, "opp_featuretool: "+FEATURESTATEFILE+" file updated.")

            self.nedfoldersExcluded = []
            for key, fs in sorted(self.featurestate.items(), key=lambda x:x[1].ord):
                enabled = False
                if fs.id in self.features and not fs.enabled:
                    self.nedfoldersExcluded.extend(self.features[fs.id].nedPackages)
            self.nedExclusionFileChanged = True

        if self.nedExclusionFileChanged:
            self.writeNedExclusions()
            self.printIfVerbose(1, "opp_featuretool: "+NEDEXCLUSIONSFILE+" file updated.")

        if self.errorOccurred:
            fail("an error ocurred")


#####################################################
tool = FeatureTool()
tool.run()
