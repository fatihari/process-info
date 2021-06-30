#include <linux/kernel.h> //for KERN_INFO
#include <linux/fs.h> //for filp*
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/string.h>
#include <linux/slab.h> // for kmalloc
#include <linux/module.h> //for all modules
#include <asm/uaccess.h>
#include <linux/fcntl.h> 
#include <asm/unistd.h>
#include <linux/unistd.h>
#include <linux/printk.h>
#include <linux/sched.h> //to get processes

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

int stringToInt(char input[])
{
	int i, digit, j=1, result=0;
	
	for(i=strlen(input)-1;i>=0;i--)
	{
		digit=input[i];
		digit=digit-48;
		result=result+(digit*j);
		j=j*10;
	}
	return result;
}

void cmdlineinfo(char *filename, char* proc_id)  //proc_id'yi yalnızca printk ile yazdırmak için kullanacağız.
{
	struct file *fd;
	//dosyada bilgilerin tutulduğu arabellek stringi tanımlarız. çıktımızın boyutu genellikle 30'dan da küçük, 
	//fakat bazı pid'lerde en fazla 620 karakter okunduğu gözlenmiştir. Garanti olması açısından 650 boyutlu verdik.
	char buf[650]; 
	mm_segment_t fs;
	int i;
	
	for(i=0;i<650;i++)
		buf[i] = 0;
	
	fd = filp_open(filename, O_RDONLY, 0); //RDONLY yani salt okunur anlamı var. Yalnızca veriyi okuyacağız.
	
	if(fd == NULL)
		printk(KERN_ALERT "file could not open.\n");
	
	else
	{
		fs = get_fs();
		set_fs(get_ds());
		fd->f_op->read(fd, buf, 650, &fd->f_pos); // ara bellekten 650 karakter okunur. (genellikle daha az)
		set_fs(fs);
		printk(KERN_INFO "%s\t%s\n",proc_id, buf); // kernele yazdırılır.
	}
	filp_close(fd,NULL); //acilan dosyayı okuma islemi bittikten sonra kapatırız, fonksiyondan çıkarız.

}


void statusinfo(char *filename)
{	

	struct file *fd;
	char *buf = (char *) vmalloc(300); 
	mm_segment_t fs;
	int i;
	char *temp;
	for(i=0;i<300;i++)
		buf[i] = 0;
	
	fd = filp_open(filename, O_RDONLY, 0); //RDONLY yani salt okunur anlamı var. Yalnızca veriyi okuyacağız.
	
	if(fd == NULL)
		printk(KERN_ALERT "file could not open.\n");
	
	else
	{
		fs = get_fs();
		set_fs(get_ds());
		fd->f_op->read(fd, buf, 300, &fd->f_pos); // ara bellekten 300 karakter okunur. (genellikle daha az)
		set_fs(fs);
		//printk(KERN_INFO "%s\t%s\n",proc_id, buf); // kernele yazdırılır.
		
		temp = strsep(&buf," \n"); // 1-"Name" info. We take it. 
        	printk("==================================\n%s\n",temp); // We print the "Name" information.
		
		temp = strsep(&buf," \n"); //"Unmask". This line is not the information we are looking for. Go to the next line. 
		
		temp = strsep(&buf," \n"); // 2-"State" information. We take it. 
		printk("%s\n",temp); // We print the "State" information.
		
		temp = strsep(&buf," \n"); //"Tgid"
		temp = strsep(&buf," \n"); //"Ngid"
		temp = strsep(&buf," \n"); //"Ngid"
		
		temp = strsep(&buf," \n"); // 3-"Pid" information. We take it. 
		printk("%s\n",temp); // We print the "Pid" information.
		
		temp = strsep(&buf," \n"); // 4-"PPid" information. We take it. 
		printk("%s\n",temp); // We print the "PPid" information.
		
		temp = strsep(&buf," \n"); //"TracerPid"
		
		temp = strsep(&buf," \n"); // 5-"Uid" information. We take it. 
		printk("%s\n==================================\n",temp); // We print the "Uid" information.
	}
	
	
	filp_close(fd,NULL); //acilan dosyayı okuma islemi bittikten sonra kapatırız, fonksiyondan çıkarız.
}


SYSCALL_DEFINE2(processinfo, char*, argv, char*, input_proc_id)
{	
	char pathcmdline[30]; //it is a file path for -p argument
	
	int p_id = stringToInt(input_proc_id);
	struct task_struct *task;
	//if the program was run without argument
	if (input_proc_id==NULL && argv==NULL)
	{
		printk("Right Usage:\n\t-all prints some information (process id and its argument/s) about all processes\n\t-p takes process id and prints the details of it\n\t-k takes process id\n");
		//return 0; yazarsak killed çıktısını almayabiliriz. 
	}

	//If the "-all" argument is entered
	else if (input_proc_id==NULL && strcmp(argv, "-all")==0)
	{
		printk("Process id and the argument list for the processes:\nuid\tppid\tpid\tname\tstate\t\n");
		
		//We pull the pid of each file and send it to the "statusinfo" function.    
		for_each_process(task) //
		{	
			
			char pathstatus[30]; // it is a file path for -all argument
			char strPid[5];
			sprintf(strPid, "%d", task->pid);
			
			strcpy(pathstatus, "/proc/"); //char pathstatus[30]="/proc/" diye atama yapılsaydı, ISO C90 rule hatası alıyorduk.
		
			strcat(pathstatus, strPid); //(dest,src)
			strcat(pathstatus,"/status"); //string concatenation operasyonu ile, pathstatus="/proc/[input_proc_id]/status olmuş oldu 
		
			statusinfo(pathstatus); 
			
		}
		
				
	}
	//If the "-p" argument is entered
	else if (input_proc_id!=NULL && strcmp(argv, "-p")==0)
	{
		printk("Argument#[%d]-Process Information:\n",p_id);
		printk("pid\tcommandline\n");
		strcpy(pathcmdline, "/proc/"); //char pathcmdline[30]="/proc/" diye atama yapılsaydı, ISO C90 rule hatası alıyorduk.
		
		strcat(pathcmdline, input_proc_id); //(dest,src)
		strcat(pathcmdline,"/cmdline"); //string concatenation operasyonu ile, patchcmdline="/proc/[input_proc_id]/cmdline olmuş oldu 
		
		cmdlineinfo(pathcmdline, input_proc_id); //input_proc_id değerini fonksiyonda yalnızca kernelde yazdırmak için kullanacağız.
	}

	//If the "-k" argument is entered
	else if (input_proc_id!=NULL && strcmp(argv, "-k")==0)
	{
		printk("Process [%d] was killed...\n", p_id);
	}
	return 0;
}


