def readDataFile(fileName):
    with open(fileName, "r") as f: 
        return [float(s) for s in f.read().split(",")]

