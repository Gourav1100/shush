#include "command.h"

Command::Command() { this->systemType = SystemType::UNKNOWN; }

Command::Command(SystemType systemType) : systemType(systemType) {}

void Command::execute(std::string command, bool replace) {
    this->commands.push_back(command);
    if (commands.size() > COMMAND_BUFFER_SIZE) {
        commands.erase(commands.begin());
    }
    std::string result = "";
    if (this->systemType == SystemType::WINDOWS) {
        throw Exception("Windows is not supported");
    } else if (this->systemType == SystemType::LINUX) {
        const char *args[] = {"/bin/bash", "-c", command.c_str(), NULL};
        if (replace) {
            execvp(args[0], (char *const *)args);
            return;
        }
        std::array<char, 128> buffer;
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            throw Exception("Pipe failed");
        }
        pid_t pid = fork();
        if (pid == -1) {
            throw Exception("Fork failed");
        }
        if (pid == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            dup2(pipefd[1], STDERR_FILENO);
            close(pipefd[1]);
            execvp(args[0], (char *const *)args);
            exit(0);
        } else {
            close(pipefd[1]);
            while (read(pipefd[0], buffer.data(), buffer.size()) != 0) {
                result += buffer.data();
            }
            close(pipefd[0]);
            waitpid(pid, NULL, 0);
        }
    }
    outputs.push_back(result);
}

std::string Command::get_command() { return get_command(0); }

std::string Command::get_output() { return get_output(0); }

std::string Command::get_command(int index = 0) {
    if ((long unsigned int)index >= commands.size()) {
        throw Exception("Index out of bounds");
    }
    return commands[commands.size() - (long unsigned int)index - 1];
}

std::string Command::get_output(int index = 0) {
    if ((long unsigned int)index >= outputs.size()) {
        throw Exception("Index out of bounds");
    }
    return outputs[outputs.size() - (long unsigned int)index - 1];
}