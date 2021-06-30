#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


/*---------------- "syscall_64.tbl" tablosuna eklenen satır -----------------*/
//335	common	processinfo		__x64_sys_processinfo

/*---------------- "syscalls.h" dosyasına endif'ten önce eklenen satır -----------------*/
//asmlinkage long sys_processinfo(char* argv, char* input_proc_id);

/*---------------- "/usr/src/linux-4.19.160" dizinindeki Makefile dosyasına eklenen satır -----------------*/
//core-y		+= kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ 2012510003/

/*---------------- "/usr/src/linux-4.19.160/2012510003" c dosyasının olduğu dizindeki Makefile dosyasına eklenen kod -----------------*/
//obj-y := 2012510003.o

/* Çalışmalarımı yürüttüğüm ana Kernel Sürümü: "Linux 4.19.0-8-amd64 x86_64" */
/* Derlediğim yeni Kernel Sürümü: "Linux 4.19.160-amd64 x86_64" */


void pFunc(char* input)
{	
	printf("String: %s\n", input);	
}


/*
./processinfo.out  -- 1 argc 1
./processinfo.out -all  -- argc 2
./processinfo.out -p 1425 --argc 3*/

//argc=counter(argument count), argv=vector(inputs)
int main(int argc, char* argv[])
{
	//int sysId = syscall(335);
	
	char* arg_all_p_k;
	char* arg_pid;
	long sysId;
	arg_all_p_k = argv[1];
	//arg_pid = atoi(argv[2]);
	arg_pid = argv[2];
	sysId = syscall(335, arg_all_p_k, arg_pid);
	
	
	if (argc==1)
	{
		printf("Right Usage:\n\t-all prints some information (process id and its argument/s) about all processes\n\t-p takes process id and prints the details of it\n\t-k takes process id\n");
	}
	
	else if (argc==2 && (strcmp(argv[1], "-all")==0))
	{
		printf("Process id and the argument list for the processes:\n");	
	}
	/*
	if (argc==3 && strcmp(argv[1],"-p")==0 && argv[2]!=NULL)
	{
		int process_id=atoi(argv[2]);
		int sysId = syscall(335, argv[1],process_id);
		
		printf("Argument#[%d]-Process Information: %d\n", process_id, sysId);
	}*/
	else if (argc==3 && (strcmp(argv[1],"-p") ==0 || strcmp(argv[1],"-k") ==0))
	{
		//int process_id=atoi(argv[2]);
		//int sysId = syscall(335, argv[1],process_id);
		
		pFunc(argv[1]);
		printf("Argument#[%s]-Process Information: %s. System ID: %ld\n", arg_pid, arg_all_p_k, sysId);
		
	}

	return 0;
		
}




