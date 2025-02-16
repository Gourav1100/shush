#include "../include/shush.h"

int main(int argc, char const *argv[]) {
    Shush application = Shush(SystemType::LINUX);
    try {
        application.init(argc, argv);
    } catch (const exception &e) {
        cout << bold("Error: ") << italic(e.what()) << endl;
    }
    return 0;
}