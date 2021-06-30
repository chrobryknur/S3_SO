#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	if(argc <2 ){
		printf("Brak argumentow\n");
		exit(-1);
	}
	char *ptr;
	int fd;
	struct stat statbuf;
	switch(atoi(argv[1])){
		case 1:
			printf("Odwzorowanie prywatne pliku file.txt\n"); 
			fd = open("file.txt",O_RDWR);
			fstat(fd, &statbuf);
			ptr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0); 
			printf("%s\n",ptr);
			munmap(ptr, statbuf.st_size);
			close(fd);
			break;
		case 2:
			printf("Odwzorowanie prywatne anonimowe\n");
			ptr = mmap(NULL, 64 * sizeof(char),  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			write(STDOUT_FILENO, ptr, 64);
			munmap(ptr, 64);
			break;
		case 3:
			printf("Odwzorowanie dzielone pliku file.txt\n");
			fd = open("file.txt", O_RDWR);
			fstat(fd, &statbuf);
			ptr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			printf("%s\n",ptr);
			munmap(ptr,statbuf.st_size);
			close(fd);
			break;
		case 4:
			printf("Odwzorowanie dzielone anonimowe\n");
			ptr = mmap(NULL, 64 * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
			write(STDOUT_FILENO, ptr, 64);
			munmap(ptr, 64);
			break;
	}
}
