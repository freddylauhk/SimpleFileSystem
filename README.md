# Simple FIle System (SFS)

Simple file system implementation. The SFS works on a file called HD with size 110 MB. 

### user guide
Run Makefile to build the project
```
$ ./MakeFile
```
You can run below command to test the project
```
$ ./mkfs_t
$ ./mkdir_t /root/folder1
$ ./external_cp /home/user/Documents/testfile /root/folder1/testfile
$ ./cp_t /root/folder1/testfile /root/folder1/testfile2
$ ./ls_t /root/folder1
$ ./cat_t /root/folder1/testfile2
```


### SFS Command
- mkfs_t
    - mkfs_t is used to build an simple filesystem on HD file. The command should be first step to run in order to initialize the File System.
```
$ ./mkfs_t
```

- mkdir_t *dname*
    - mkdir_t is used to create new directory file with directory name *dname*, *dname* should be in absolute path (e.g. /root). New directory will replace the old one if the directory already exist in file system.
```
$ ./mkdir_t /root
$ ./mkdir_t /root/folder1
```

- ls_t *dname*
    - ls_t is used to list the information of all files under directory *dname*, *dname* should be in absolute path (e.g. /root/folder1)
```
$ ./ls_t /root/folder1
```

- cat_t *path_name*
    - cat_t prints the contents of file using absolute path *file_path*. If the file print out bugged, please try to external_cp the file again
```
$ ./cat_t /root/folder1/file1
```

- external_cp *external_file_path* *sfs_path_path*
    - external_cp copies external file (Real machine file) to SFS path. *external_file_path* and *sfs_path_path* should be absolute path
```
$ ./external_cp /home/user/Documents/testfile /root/folder1/testfile
```

- cp_t *source_path* *destination_path*
    - cp_t copies a regular file from *source_path* to *destination_path*
```
$ ./cp_t /root/folder1/testfile /root/folder1/testfile2
```