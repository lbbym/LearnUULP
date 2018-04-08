/* spwd.c: a simplified version of pwd
 * 
 * starts in current directory and recursively
 * climbs up to the root of filesystem, print top
 * part then prints current part
 *
 * uses resddir() to get info about each thing
 * 
 * bug: prints an empty string if run from "/"
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define BUFSIZE 24

ino_t get_inode(char *);
void printpathto(ino_t);
void inum_to_name(ino_t, char *, int);


int main(int argc, char const *argv[])
{
	printpathto(get_inode("."));
	putchar('\n');
	return 0;
}

void printpathto(ino_t this_inode)
/*
 * prints path leading down to an object with this inode
 * kinodef recursive
 */
{
	ino_t my_inode;
	char its_name[BUFSIZE];
	if(get_inode("..") != this_inode)
	{
		chdir("..");									//up one dir
		inum_to_name(this_inode, its_name, BUFSIZE);	//get its name
		my_inode = get_inode(".");						//print head
		printpathto(my_inode);							//recursively
		printf("/%s", its_name);						//now print
														//name of this
	}
}

void inum_to_name(ino_t inode_to_find, char * namebuf, int buflen)
/*
 * looks through current directory for a file with this inode
 * number and copies its name into namebuf
 */
{
	DIR 	*dir_ptr;							//the directory
	struct dirent * direntp;					//each entry
	dir_ptr = opendir(".");
	if(dir_ptr == NULL)
	{
		perror(".");
		exit(1);
	}
	//search directory for a file with specified inum
	while((direntp = readdir(dir_ptr)) != NULL)
	{
		if(direntp->d_ino == inode_to_find)
		{
			strcpy(namebuf, direntp->d_name);
			namebuf[buflen-1] = '\0';			//just in case
			closedir(dir_ptr);
			return;
		}
	}
	fprintf(stderr, "error looking for inum %d\n", inode_to_find);
	exit(1);
}

ino_t get_inode(char * fname)
/*
 * return inode number of the file
 */
{
	struct stat info;
	if(stat(fname, &info) == -1)
	{
		fprintf(stderr, "Cannot stat");
		perror(fname);
		exit(1);
	}
	return info.st_ino;
}