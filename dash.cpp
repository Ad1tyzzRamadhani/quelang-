#include <iostream>
#include <cstdlib>

void showMenu() {
    std::cout << "\n=== 🌱 QueLang CLI ===\n";
    std::cout << "1. Compile file .q → .s\n";
    std::cout << "2. Compile & Assemble & Link → binary\n";
    std::cout << "3. Run example\n";
    std::cout << "4. Exit\n";
    std::cout << "Pilih menu: ";
}

int main() {
    int choice;
    std::string input, output;

    while (true) {
        showMenu();
        std::cin >> choice;
        if (choice == 1) {
            std::cout << "Nama file input (.q): ";
            std::cin >> input;
            std::cout << "Nama file output (.s): ";
            std::cin >> output;
            std::string cmd = "./quelang " + input + " " + output;
            system(cmd.c_str());
        } else if (choice == 2) {
            std::cout << "Nama file input (.q): ";
            std::cin >> input;
            std::string asmFile = "output.s";
            std::string objFile = "out.o";
            std::string binFile = "binary";
            std::string compile = "./quelang " + input + " " + asmFile;
            std::string assemble = "aarch64-linux-gnu-as -o " + objFile + " " + asmFile;
            std::string link = "aarch64-linux-gnu-ld -o " + binFile + " " + objFile;
            system(compile.c_str());
            system(assemble.c_str());
            system(link.c_str());
        } else if (choice == 3) {
            std::cout << "Running example.q ...\n";
            system("./quelang example.q output.s");
            system("aarch64-linux-gnu-as -o out.o output.s");
            system("aarch64-linux-gnu-ld -o binary out.o");
        } else if (choice == 4) {
            std::cout << "Bye!\n";
            break;
        } else {
            std::cout << "Menu tidak valid!\n";
        }
    }

    return 0;
}
