//Родительский процесс создает два дочерних процесса. Первой строкой 
//пользователь в консоль родительского процесса вводит имя файла, которое 
//будет использовано для открытия File с таким именем на запись для child1. 
//Аналогично для второй строки и процесса child2. 
//Родительский процесс принимает от пользователя строки произвольной 
//длины и пересылает их в pipe1 или в pipe2 в зависимости от правила 
//фильтрации. Процесс child1 и child2 производят работу над строками. 

// Правило фильтрации: с вероятностью 80% строки отправляются в pipe1, иначе 
// в pipe2. Дочерние процессы инвертируют строки.

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


void reverse(char name[]) // разворачивает строки
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
 
void child_work(int from, int to) {
  	char buf;
  	char string[100];
  	int i = 0;
 
  	while (read(from, &buf, 1) > 0) {
		string[i++] = buf;
		string[i] = '\0';
		if ((buf == '\n')||(buf == '\r')) {
			reverse(string);
			write(to, string, i);
			i = 0;
		}	
  	}
 
  	close(to);
  	close(from);
}

void parrent_work(int child1, int child2) {
  char buf[1];
  srand(time(NULL));
  int n = (rand() % 10) + 1;
  while (read(STDIN_FILENO, buf, 1) > 0) {
    if (n > 2) {
      write(child1, buf, 1);
    } else {
      write(child2, buf, 1);
    }
    if ((buf[0] == '\n')||(buf[0] == '\r')) {
      n = (rand() % 10) + 1;
    }
  }

  close(child1);
  close(child2);
}

int read_name_and_open_file() {
  const size_t FILE_NAME_SIZE = 64;
  char f_name[FILE_NAME_SIZE];
  char buf[1];
  int idx = 0;
  while (idx < FILE_NAME_SIZE && read(STDIN_FILENO, buf, 1) > 0) { 
	  // reed(файловый дескриптор, буфер для записи, число байт)
	  // STDIN_FILENO - поток ввода(терминал)
    if (buf[0] == '\n') {
      break;
    }
    f_name[idx++] = buf[0];
  }
  f_name[idx++] = '\0';
  return open(f_name, O_WRONLY | O_TRUNC);
  // open(имя файла(путь), режим доступа)
  // O_WRONLY - только запись
  // O_TRUNC - очистить при необходимости

}

void error(char* buf, size_t size) { // посимвольно выводит сообщение об ошибке
   	write(STDERR_FILENO, buf, size);
	// write(файловый дескриптор, буфер из которого читается, число байт)
	// STDERR_FILENO - поток диагностики(вывода ошибок)
}

void check_file_id(int id) {
  if (id == -1) {
    error("File not found\n", 15);
    exit(-1);
  }
}

void check_pipe_creation(int* pipefd) {
  	if (pipe(pipefd) == -1) { 
  	  	error("Can not create pipe\n", 20);
  	  	exit(-2);
  	}
}

int check_fork() {
  	int fd = fork();
  	if (fd == -1) {
  	  	error("Can not create process\n", 23);
  	  	exit(-3);
  	}
  	return fd;
}

int main(int argc, char* argv[]) {
	int f1 = read_name_and_open_file(); 
 	check_file_id(f1);
  	int f2 = read_name_and_open_file();
  	check_file_id(f2);

  	int pipefd1[2];
  	check_pipe_creation(pipefd1);

  	int child1 = check_fork();
  	if (child1 == 0) {
  	 	close(pipefd1[1]);
  	  	child_work(pipefd1[0], f1);
  	  	return 0;
  	}
  	close(pipefd1[0]);

  	int pipefd2[2];
  	check_pipe_creation(pipefd2);

  	int child2 = check_fork();
  	if (child2 == 0) {
  	  	close(pipefd1[1]);
  	  	close(pipefd2[1]);
  	  	child_work(pipefd2[0], f2);
  	  	return 0;
  	}
  	close(pipefd2[0]);

  	parrent_work(pipefd1[1], pipefd2[1]);

  	return 0;
}
