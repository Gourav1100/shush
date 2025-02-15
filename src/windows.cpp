#include "../include/shush.h"

int main(int argc, char const *argv[])
{
    Shush application = Shush(SystemType::WINDOWS);
    try
    {
        application.init(argc, argv);
    }
    catch (Exception e)
    {
        cout << bold("Error: ") << italic(e.what()) << endl;
    }
    return 0;
}
