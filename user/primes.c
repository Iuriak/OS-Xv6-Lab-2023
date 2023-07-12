#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  printf("primes has been called \n");

  int first, v;
  // 用于在父进程和子进程之间交换管道文件描述符。
  int *fd1;
  int *fd2;
  // 创建第一个管道和之后的管道
  int first_fd[2];
  int second_fd[2];

  pipe(first_fd);

  // 创建父进程，将数字2-35输入管道
  if (fork() > 0)
  {
    for (int i = 2; i <= 35; i++)
      write(first_fd[1], &i, sizeof(i));
    // 输入后关闭写
    close(first_fd[1]);
    // 等待子进程结束
    int status;
    wait(&status);
  }
  // 对于子进程，循环处理，为素数创建新进程
  else
  {
    // 子进程从 first_fd 管道中读取数据，并创建一个新的管道 fd2
    fd1 = first_fd;
    fd2 = second_fd;

    while (1)
    {
      // 创建一个管道，为下一次读取做准备。
      pipe(fd2);
      // 子进程关闭了前一个管道的写端，从管道 fd1 中读取第一个值(素数)并打印
      close(fd1[1]);
      if (read(fd1[0], &first, sizeof(first)))
        printf("prime %d\n", first);
      else
        break;
      // 子进程遍历读取 fd1 管道的剩余值，对每个值进行判断
      if (fork() > 0)
      {
        int i = 0;
        while (read(fd1[0], &v, sizeof(v)))
        {
          // 如果可以整除 first（当前素数），则跳过该值，继续下一个
          if (v % first == 0)
            continue;
          i++;
          // 如果不可以整除，则将该值写入 fd2 管道中，以供下一个子进程使用。
          write(fd2[1], &v, sizeof(v));
        }
        // 关闭fd1的读端和fd2的写端，并等待子进程结束
        close(fd1[0]);
        close(fd2[1]);
        int status;
    	wait(&status);
        break;
      }
      // 子进程之间数据的传递
      else
      {
        close(fd1[0]);//close fd1 read
        int *tmp = fd1;
        fd1 = fd2;
        fd2 = tmp;
      }
    }
  }
  //父进程在子进程结束后退出
  exit(0);
}
