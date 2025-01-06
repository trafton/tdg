#include <libtdb/libtdb.hpp>
#include <iostream>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <editline/readline.h>
#include <string>

namespace {
	void handle_command(pid_t pid, std::string_view line);
	pid_t attach(int argc, const char** argv) {
		pid_t pid = 0;

		// PID
		if(argc == 3 && argv[1] == std::string_view("-p")) {
			pid = std::atoi(argv[2]);
			if(pid <= 0) {
				std::cerr << "Invalid pid" << std::endl;
				return -1;
			}

			if(ptrace(PTRACE_ATTACH, pid, /*addr=*/nullptr, /*data=*/nullptr) < 0) {
				std::perror("Could  not attach");
				return -1;
			}

		} 
		// Program Name
		else {
			const char* program_path = argv[1];

			if((pid = fork()) < 0) {
				std::perror("Fork failed.");
				return -1;
			}

			if(pid == 0) {
				// in child process
				// Execute debugee.

				if(ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0) {
					std::perror("Tracing failed.");
					return -1;
				}
				if(execlp(program_path, program_path, nullptr) < 0) {
					std::perror("Exec failed.");
					return -1;
				}
			}
		}

		return pid;
	}
}

int main(int argc, const char** argv) {
	if(argc == 1) {
		std::cerr << "No arguments given" << std::endl;
		return -1;
	}

	pid_t pid = attach(argc, argv);

	int wait_status;
	int options = 0;

	if(waitpid(pid, &wait_status, options) < 0) {
		std::perror("waitpid failed");
	}

	char* line = nullptr;
	while((line = readline("tdb> ")) != nullptr) {
		std::string line_str;

		if(line == std::string_view("")) {
			free(line);
			if(history_length > 0) {
				line_str = history_list()[history_length - 1]->line;
			}
		} else {
			line_str = line;
			add_history(line);
			free(line);
		}

		if(!line_str.empty()){
			handle_command(pid, line_str);
		}

	}
}