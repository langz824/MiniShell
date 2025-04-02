#include <iostream> 
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

// 此為拆分string 命令本身 與 參數
std::vector<std::string> splitCommand(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
        return tokens;
    }
}



int main() {
    std::string input;

    while (true) {
        std::cout << "[minishell]$";
        std::getline(std::cin, input); 

        if (input == "exit") {
            break;
        }

        // 此為拆分的函數引入
        std::vector<std::string> tokens = splitCommand(input); 

        // std::cout << "你輸入的是" << input << std::endl; 最一開始在用的

        std::vector<char*> args;
        for (auto& token : tokens) {
            args.push_back(&token[0]);
        }
        args.push_back(nullptr);

        pid_t pid = fork();
        if (pid == 0) {
            // 子進程：執行命令
            execvp(args[0], args.data());
            // 如果execvp 返回，表示執行失敗
            perror("execvp failed");
            exit(1);
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            // fork 失敗
            perror("fork failed");
        }
    }

    return 0;
}