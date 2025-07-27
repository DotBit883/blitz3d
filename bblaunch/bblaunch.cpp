#include <filesystem>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  std::filesystem::path current_path = std::filesystem::current_path();
  setenv("blitzpath", current_path.c_str(), 0);
  std::filesystem::current_path(current_path);

  pid_t pid = fork();

  if (pid == 0) {
    std::filesystem::path ide_path = current_path / "bin/ide.exe";
    execvp(ide_path.c_str(), argv);

    std::cerr << "Failed to execute program." << std::endl;
    _exit(1);
  } else if (pid > 0) {
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to complete
    std::cout << "Child process exited with status " << status << std::endl;
  } else {
    std::cerr << "Failed to fork process." << std::endl;
  }

  return 0;
}
