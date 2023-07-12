#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int parent_fd[2]; 	// File Descriptors for Storage Pipes
  int child_fd[2];
  pipe(parent_fd);	// Calling pipe creates a parent pipe
  pipe(child_fd);	// Calling pipe creates a child pipe

  // Creating child processes with fork
  if (fork() == 0)
  {
    char buf[10];			// For storing read data
    read(parent_fd[0], buf, sizeof buf); // Read data from parent_fd[0], read data stored in buf
    int id = getpid();			// Get the ID of the current process and print it
    printf("%d: received %s\n", id, buf);
    write(child_fd[1], "pong", 4);	// Write string "pong" to child_fd[1] pipe
    // Close the read side and write side of child_fd.
    close(child_fd[0]);
    close(child_fd[1]);
  }
  else
  {
    char buf[10];
    int id = getpid();
    write(parent_fd[1], "ping", 4);	
    // Write the string "ping" to the parent_fd[1] pipe and close the write side of that pipe
    close(parent_fd[1]);
    int status;
    wait(&status); 
    read(child_fd[0], buf, sizeof buf);
    printf("%d: received %s\n", id, buf);
    close(child_fd[0]);
  }

  exit(0);
}
