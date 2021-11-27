#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int is_it_time_to_terminate = 0;
const int OFFSET = sizeof(int);

void update_is_it_time_to_terminate(int num) {
  is_it_time_to_terminate = 1;
}

void reverse(char name[])
{
    char tmp;
    int len = strlen(name);
	--len;
    for (int i = 0; i < len; i++) {
        tmp = name[len - 1];
        name[len - 1] = name[i];
        name[i] = tmp;
        len--;
    }
}

void child_work(char* from, int to) {
    int idx = OFFSET;
    char ch;
    char string[100];
  	int i = 0;
  while (1) {
    if (idx < OFFSET + ((int*)from)[0]) {
      string[i++] = from[idx];
      ch = from[idx];
      string[i] = '\0';
      if ((ch == '\n')||(ch == '\r')){
        reverse(string);
		write(to, string, i);
		i = 0;
      }
      ++idx;
    } else {
      if (is_it_time_to_terminate) {
        break;
      }
    }
  }
  close(to);
}

void parrent_work(pid_t child1, char* child_map1, pid_t child2, char* child_map2) {
  char ch;
  int idx1 = OFFSET;
  int idx2 = OFFSET;
  srand(time(NULL));
  int n = (rand() % 10) + 1;
  while (read(STDIN_FILENO, &ch, 1) > 0) {
    if (n > 2) {
      child_map1[idx1++] = ch;
    } else {
      child_map2[idx2++] = ch;
    }
    if (ch == '\n') {
      if (n > 2) {
        ((int*)child_map1)[0] = idx1 - OFFSET;
      } else {
        ((int*)child_map2)[0] = idx2 - OFFSET;
      }
      n = (rand() % 10) + 1;
    }
  }

  kill(child1, SIGUSR1);
  kill(child2, SIGUSR1); //сигнал о зовершении работы родительского процесса
  int res1;
  int res2;
  waitpid(child1, &res1, 0); // ждем завершения дочерних процессов
  waitpid(child2, &res2, 0);
  if (res1 != 0 || res2 != 0) {
    fprintf(stderr, "Something ended ne tak!\n%d %d\n", res1, res2);
  }
}


int read_name_and_open_file() {
  const size_t FILE_NAME_SIZE = 64;
  char f_name[FILE_NAME_SIZE];
  char buf[1];
  int idx = 0;
  while (idx < FILE_NAME_SIZE && read(STDIN_FILENO, buf, 1) > 0) {
    if (buf[0] == '\n') {
      break;
    }
    f_name[idx++] = buf[0];
  }
  f_name[idx++] = '\0';
  return open(f_name, O_WRONLY | O_TRUNC);
}

void error(char* buf, size_t size) { write(STDERR_FILENO, buf, size); }
void check_file_id(int id) {
  if (id == -1) {
    error("File not found\n", 15);
    exit(-1);
  }
}
void* check_map_creation() {
  void* m_file = mmap(NULL, 2048, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  //  mmap  ( void * address , size_t  length , int  protect , int  flags , int  filedes(дискриптор) ,off_t  offset(смещение))
  // 2048 байт нужно отобразить в память
  // PROT_READ | PROT_WRITE - права на чтение и запись
  // MAP_SHARED  - флаг для совместного использования отображения со всеми другими процессами, которые отображаются на этот объект
  // MAP_ANONYMOUS - флаг для создания анонимного сопоставления (файл отображаемый в память будет виртуальным)
  if (m_file == MAP_FAILED) { // сбой (иначе 0)
    error("Cannot create mmap\n", 19);
    exit(-2);ы
  }
  ((int*)m_file)[0] = 0;
  return m_file;
}
int check_fork() {
  int fd = fork();
  if (fd == -1) {
    error("Cannot create process\n", 22);
    exit(-3);
  }
  return fd;
}
void add_signals() {
  void (*func)(int);
  func = signal(SIGUSR1, update_is_it_time_to_terminate);
  // SIGUSR1 - сигнал отданный на распоряжение пользователю
  if (func == SIG_IGN) { // сигнал игнорируется
    error("Cannot add signal\n", 18);
    exit(-4);
  }
}

int main(int argc, char* argv[]) {
  add_signals();

  int f1 = read_name_and_open_file();
  check_file_id(f1);
  int f2 = read_name_and_open_file();
  check_file_id(f2);

  char* m_file1 = check_map_creation();
  pid_t child1 = check_fork();
  if (child1 == 0) {
    close(f2);
    child_work(m_file1, f1);
    return 0;
  }
  close(f1); // закрывает mmap для f1

  char* m_file2 = check_map_creation();
  pid_t child2 = check_fork();
  if (child2 == 0) {
    child_work(m_file2, f2);
    return 0;
  }
  close(f2); // закрывает mmap для f2

  parrent_work(child1, m_file1, child2, m_file2);

  return 0;
}
