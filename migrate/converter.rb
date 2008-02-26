warningTable = {
    'cPolymorphic\s+\*dup\(\)' => 'dup() to be upgraded',
    'cObject\s+\*dup\(\)' => 'dup() to be upgraded',
    'cPar +[a-z]' => 'cPar used as value object, revise!',
    'new +cPar' => 'change cPar to cMsgPar'
}

renameTable = {
    'cException' => 'cRuntimeError',
    'getTail' => 'pop',
    'getString' => 'toString',
    'runningModule' => 'activityModule',
    'tail' => 'front'
}


#recipe = File.open("todo.txt", "w")
File.open("filelist.txt").each_line { |filename|
    filename = filename.chop
    contents = File.open(filename).read

    warningTable.each_key { |key|
        if contents =~ /#{key}/
            puts filename+": "+warningTable[key]
            #recipe << "\t/"+key+"/ ====> ???\n"
        end
    }

    renameTable.each_key { |key|
        if contents =~ /\b#{key}\b/
            puts filename+": change `"+key+"' to `"+renameTable[key]+"'"
            #recipe << "\t/\\b"+key+"\\b/ ====> " + renameTable[key] + "\n"
        end
    }
}