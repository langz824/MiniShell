#include <iostream> // 包含 cin, cout, endl
#include <string> 

int main() {
    std::string input;

    while (true) {
        std::cout << "[minishell]$";
        std::getline(std::cin, input); // 包含空格的整行讀取

        if (input == "exit") {
            break;
        }

        std::cout << "你輸入的是：" << input << std::endl;
    }

    return 0;
}