# PopProject
Principle of programming project.

A File Maker program which can Create, Edit, Manage forms.

It's recommended to make program by Cmake so, for compiling on unix based operation systems by cmake run commands respectively:

First create a bulid directory where you cloned repository;
```
mkdir build-dir
```
Then go into build directory;
```
cd build-dir
```
Then give Cmake the top directory where .c and .h files exists as modules;
```
cmake ..
```
At the end make programm by make command at build directory;
```
make
```

**Note that the program would make two FormsTemplates and FormsData directories where it has been built;**

**Which would be used for saving Templates and all of Data respectively.**
