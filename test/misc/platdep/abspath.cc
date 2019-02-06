#include <iostream>
#include <common/fileutil.h>

int main(int argc, char **argv)
{
    if (argc == 2)
        std::cout << argv[1] << " --> " << toAbsolutePath(argv[1]) << "\n";
    else if (argc == 3)
        std::cout << argv[1] << " + " << argv[2] << " --> " << concatDirAndFile(argv[1], argv[2]) << "\n";
    else
        std::cout << "error: supply 1 or 2 parameters!\n";
    return 0;
}

