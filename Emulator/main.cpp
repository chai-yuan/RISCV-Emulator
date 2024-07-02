#include "Machine.hpp"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        ERROR("需要参数");
    }

    auto machine = Machine(argv[1]);
    machine.gdbRun();
}
