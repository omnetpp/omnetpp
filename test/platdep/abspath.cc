#include <platdep/fileutil.h>

int main(int argc, char **argv)
{
    if (argc<2)
        return 1;
    std::cout << absolutePath(argv[1]) << "\n";
    return 0;
}

