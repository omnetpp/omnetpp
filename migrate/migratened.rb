# http://www.germane-software.com/software/rexml/docs/tutorial.html
# http://www.rubycentral.com/book/tut_stdtypes.html

require "rexml/document"

#helper functions
# returns the firs param if it is not null otherwise the second
def nvl(par1, par2)
    if par1.nil?
        par2
    else
        par1
    end
end
# gets an element or creates one if does not exists (returns the requested element)
def createOrGetElement(parent, whereXPath, tagName, attributes)
    parent.elements.each(whereXPath) { |node|
        return node
    }
    return parent.add_element(tagName, attributes)
end

#####################################################################################################
# open and parse the new document
puts "Starting conversion"
#source = File.open("inetframework.xml").read
doc = REXML::Document.new(File.open("inetframework.xml"))
puts "Documet parsed"

#write out the whole xml to be able to compare wuth the results
doc.write(File.open("step0.xml", "w"))

# create module hash table for later use
moduleHash = {}
doc.elements.each("*/*/compound-module | */*/simple-module") { |mod|
    moduleHash[mod.attributes["name"]] = mod
}

#####################################################################################################
# remove extra "network bla extends Bla {}" if needed; this affects ini files as well!
puts "Starting toplevel network removal"
# a replacement hashtable
replaceTable = {}
doc.elements.each("*/*/compound-module[@is-network='true']") { |network|
    # who is extended by this module
    extends = network.elements[1,"extends"].attributes["name"]
    # the name of this network module
    networkName = network.attributes["name"]
    puts "Processing: "+network.attributes["name"]+" extends #{extends}"
    doc.elements.each("*/*/compound-module[@name='#{extends}'] | */*/simple-module[@name='#{extends}']") { |modified_module|
        modified_module.attributes["is-network"]="true"
        # store the name and newName in a hashtable
        replaceTable[networkName] = extends
    }
    network.remove
}

doc.write(File.open("step1.xml", "w"))

#replace network names in all ini files
File.open("inilist.lst").each_line { |filename|
    filename = filename.chop
    contents = File.open(filename).read

    replaceTable.each_key { |key|
        if contents.gsub!(/\b#{key}\b/ , replaceTable[key])
            puts "replaced: "+ key+" => "+replaceTable[key]+" in "+filename
        end
    }

    File.open(filename, "w").write(contents)
}

###############################################################################################
# "somepar = input;" lines became just "somepar;" -- remove them
puts "Removing redundant parameters"
doc.elements.each("//param[not(@type) and not(@value) and not(*)]") { |param|
    param.remove
}
doc.write(File.open("step2.xml", "w"))

###############################################################################################
# make use of default icons: migrate "i=" tags from submodule display string to the simple module type
puts "Setting default icons for modules"
# {submoduleType => {iconName => occurances}}
type2iconMap = {}

# look into each submodule and get the display property icon value
doc.elements.each("//submodule") { |submodule|
    submodType = submodule.attributes["type"]

    submodule.elements.each("parameters/property[@name='display']/property-key/literal") { |displayliteral|
        displaystring = displayliteral.attributes["value"];
        if displaystring =~ /i=([a-zA-Z0-9_\/]+)/
            icon = $1
            if type2iconMap[submodType].nil?
                type2iconMap[submodType] = {}
            end
            if type2iconMap[submodType][icon].nil?
                type2iconMap[submodType][icon] = 0
            end

            type2iconMap[submodType][icon] += 1
        end
    }
}

# write the most common values back to the simple or compound modules (ie. in the submodule's type)
type2iconMap.each_pair { |typeName, iconsMap|
    icon = iconsMap.index(iconsMap.values.sort.last)
    puts "Recommending #{icon} for #{typeName}"
    p iconsMap
    # insert display string into the type
    doc.elements.each("*/*/compound-module[@name='#{typeName}'] | */*/simple-module[@name='#{typeName}']") { |modified_module|
        puts "Processing module: #{modified_module.attributes['name']}"

        # parameters block must be before gates connections and types
        parameters = modified_module.elements["parameters"]
        if parameters.nil?
            removedElements = modified_module.elements.delete_all("gates | types | submodules | connections")
            modified_module.add_element("parameters")
            removedElements.each { |element|
                modified_module.add_element(element)
            }
        end


        parameters = createOrGetElement(modified_module,"parameters", "parameters", {})
        property = createOrGetElement(parameters,"property[@name='display']","property", {"name"=>"display"})
        propertykey = createOrGetElement(property,"property-key","property-key", {})
        literal = createOrGetElement(propertykey,"literal","literal", {"type"=>"string", "value"=> ""})
        literal.attributes["value"] += "i=#{icon}"
    }
}

#array of types where the original icon should be kept in submodules
exceptionTypes = ["StandardHost"]
#delete icon information from submodules where we have specified a default icon name in the type itself
doc.elements.each("//submodule") { |submodule|
    submodType = submodule.attributes["type"]
    if !exceptionTypes.include?(submodType)
        submodule.elements.each("parameters/property[@name='display']/property-key/literal") { |displayliteral|
            displaystring = displayliteral.attributes["value"];
            # remove icon definitions
            displaystring.gsub!(/i=([a-zA-Z0-9_\/]+)/, "")
            displaystring.gsub!(/;$/, "")
            displaystring.gsub!(/^;/, "")
            displaystring.gsub!(/;;/, ";")
            displayliteral.attributes["value"] = displaystring;
            displayliteral.attributes["text"] = ""
        }
    end
}

doc.write(File.open("step3.xml", "w"))

#############################################################################################################
# set default parameter values in simple and compound modules
puts "Setting default parameters for modules"
defaultParamsFile = File.new("defaultparams.txt")

# set the unit value (add a property to the parameter)
def setUnitProperty(node, unit)
    property = createOrGetElement(node, "property[@name='unit']", "property", {"name"=>"unit"})
    propkey = createOrGetElement(property, "property-key", "property-key", {})
    literal = createOrGetElement(propkey, "literal", "literal", {"type"=>"string", "value"=>unit})
end

#change the module parameters to have default value
moduleName = ""
defaultParamsFile.each_line { |line|
    a = line.chomp.split(/ *\t */)
    next if a.length == 0
    moduleName = a[0] if a.length == 1
    if a.length > 1
        paramName = a[1]
        paramType = a[2]
        paramValue = nvl(a[3], "")
        paramUnit = nvl(a[4], "")
        puts "Processing: #{moduleName}"
        if !moduleHash[moduleName].nil?
            moduleHash[moduleName].elements.each("parameters/param[@name='#{paramName}']") { |paramNode|
                paramNode.attributes['value'] = paramValue unless (paramValue == "-" || paramValue == "TBD")
                paramNode.attributes['type'] = paramType
                paramNode.attributes['is-default'] = "true"
                setUnitProperty(paramNode, paramUnit) unless paramUnit == ""
            }
        end
    end
}

###################################################################################################
# prostprocess the result file
puts "Postprocessing file"
result = ""
doc.write(result)

result.gsub!("&amp;", "&")
result.gsub!("& ", "&amp;")
result.gsub!("&&", "&amp;&amp;")

resultfile = File.open("result.xml","w")
resultfile.write(result)
resultfile.close
