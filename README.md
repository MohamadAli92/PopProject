# PopProject

Principle of programming project.

A File Maker program which can Create, Edit, Manage forms implemented using pure C and ncurses.h library.

This project doesn’t have a GUI interface but I have used ncurses to develop a stunning TUI interface which is nothing short of GUI because I have implemented all of the functionalities and menus like a GUI interface 😊.

-   **Main Menu:**

    This application has 3 functionalities:

1.  Create form templates
2.  Manage and fill out different samples of form templates.
3.  Edit both templates and filled out forms.

    You can choose what to do from main menu using arrow keys.

![](media/d794095252922d04b7fc34b8331157a3.png)

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
