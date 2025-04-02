/*
 * minishell 功能說明（2025/04/02 提交版本）
 *
 *   支援功能：
 * - 輸出重定向（>）：可將指令輸出寫入指定檔案
 * - 輸入重定向（<）：可從指定檔案讀取輸入內容
 * - 單層管線（|）：可將前一指令的輸出，作為後一指令的輸入（如 ls | grep txt）
 *
 *   實作方式：
 * - 使用 open() 開啟檔案並搭配 dup2() 重定向 STDIN / STDOUT
 * - 使用 pipe() 建立管線，搭配兩個 fork() 分別執行左右指令
 * - 每個子進程皆關閉不需要的 pipe 端以避免阻塞
 * - 父進程使用 waitpid() 等待兩個子進程結束
 *
 *
 *   可執行範例：
 *   ls
 *   ls > out.txt
 *   cat < out.txt
 *   cat < input.txt | grep hello
 *
 */

#include <iostream> 
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


// 此為拆分string 命令本身 與 參數
std::vector<std::string> splitCommand(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}



int main() {
    std::string input;

    while (true) {
        std::cout << "[minishell]$ ";
        std::getline(std::cin, input); 

        if (input == "exit") {
            break;
        }

        // 此為拆分的函數引入
        std::vector<std::string> tokens = splitCommand(input); 
        std::vector<char*> args;
        
        // 在輸入系統指令前，可使用 string，不用去處理記憶體的問題，像是結束後要釋放的部分
        std::string outputfile, inputfile;
        // 先檢查pipe "|" 是否在tokens裡
        bool haspipe = false;
        size_t pipindex;
        for (size_t i = 0; i < tokens.size(); i++) {
            if ( tokens[i]== "|" && i+1 < tokens.size()) {
                haspipe = true;
                pipindex = i;
                break;
            }
        }

        if (haspipe) {
            std::vector<std::string> tokens1(tokens.begin(), tokens.begin() + pipindex);
            std::vector<std::string> tokens2(tokens.begin() + pipindex +1, tokens.end());
            std::vector<char*> args1, args2;

            for (size_t i = 0; i < tokens1.size(); i++) {
                args1.push_back(&tokens1[i][0]);  
            }
            
            for (size_t i = 0; i < tokens2.size(); i++) {
                args2.push_back(&tokens2[i][0]);  
            }
            

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe failed");
                exit(1);
            }
            pid_t pid1 = fork();

            if (pid1 == 0) {
                // 子進程：執行命令
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                execvp(args1[0], args1.data());
                // 如果execvp 返回，表示執行失敗
                perror("execvp failed");
                exit(1);
            } else if (pid1 > 0) {
                // int status; 不要在這裡waitpid
                // waitpid(pid1, &status, 0);
            } else {
                // fork 失敗
                perror("fork failed");
            }
            pid_t pid2 = fork();
            if (pid2 == 0) {
                // 子進程：執行命令
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[1]);
                close(pipefd[0]);
                execvp(args2[0], args2.data());
                // 如果execvp 返回，表示執行失敗
                perror("execvp failed");
                exit(1);
            } else if (pid2 > 0) {
                int status;
                close(pipefd[0]);
                close(pipefd[1]);
                waitpid(pid1, &status, 0);
                waitpid(pid2, &status, 0);
            } else {
                // fork 失敗
                perror("fork failed");
            }
        } else {
        // 此為沒 "|"情形
        // 檢查 '>', '<' 情境，實現輸出、輸入的重新定向
            for (size_t i = 0; i < tokens.size(); i++) {
                if( tokens[i] == ">" && i+1 < tokens.size()) {
                    outputfile = tokens[i+1];
                    i++;
                }
                else if( tokens[i] == "<" && i+1 <tokens.size()) {
                    inputfile = tokens[i+1];
                    i++;
                }
                else {
                    args.push_back(&tokens[i][0]);  
                }
            }

            pid_t pid = fork();
            if (pid == 0) {
                // 子進程：執行命令
                if (!inputfile.empty()) { // 輸入
                    int fd = open(inputfile.c_str(), O_RDONLY);
                    if (fd < 0) {
                        perror("open failed");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (!outputfile.empty()){
                    // 此開啟檔案使用到 只寫、沒有的話建立、有內容的話覆蓋
                    int fd = open(outputfile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open failed");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
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
        
    }

    return 0;
}