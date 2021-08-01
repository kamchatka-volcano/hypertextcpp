<p align="center">
  <img src="doc/logo.png"/>  
</p>

**hypertextcpp** - is a hyperfast HTML templating system for C++ applications.  
It provides a highly readable `.htcpp` template file format and a command line utility that transpiles it to C++ HTML rendering code. Include a generated C++ header file in your project, then setup a build system to update it when necessary, and you're all set.

A quick example:

[`examples/01/todolist.htcpp`](examples/01/todolist.htcpp)
```html
<html>	
    <body>
        <h1>$(cfg.name)'s todo list:</h1>
        <p> No tasks found </p>?(cfg.tasks.empty())
        <ul>
            <li>$(task.name)</li>@(auto task : cfg.tasks)
        </ul>
    </body>
</html>

```

Now let's generate the C++ header:
```console
kamchatka-volcano@home:~$ hypertextcpp todolist.htcpp
```

This command creates the `todolist.h` file in the current working directory. All that's left is to use it in our program.

[`examples/01/todolist_printer.cpp`](examples/01/todolist_printer.cpp)
```cpp
#include "todolist.h"
#include <string>
#include <vector>

int main()
{	
    struct PageParams{
        struct Task{
            std::string name;
        };
        std::string name = "Bob";
        std::vector<Task> tasks = {{"laundry"}, {"cooking"}};
    } pageParams;
    auto page = todolist{};
    page.print(pageParams);
    return 0;
}

```

Compile it with your preferred method, launch it, and you will get this output:
```console
kamchatka-volcano@home:~$ ./todolist_printer
<html>	
    <body>
        <h1>Bob's todo list:</h1>
        
        <ul>
            <li>laundry</li><li>cooking</li>
        </ul>
    </body>
</html>

```


## Table of contents
* [Template file syntax](#template-file-syntax)
    * [Expressions](#expressions)
    * [Statements](#statements)
    * [Global statements](#global-statements)
    * [Control flow extensions](#control-flow-extensions)
    * [Sections](#sections)
    * [Procedures and partial rendering](#procedures-and-partial-rendering)
* [Code generation](#code-generation)
    * [Command line parameters](#command-line-parameters)
    * [Single header renderer](#single-header-renderer)
    * [Shared library renderer](#shared-library-renderer)
* [Installation](#installation)
* [Running tests](#running-tests)
* [License](#license)

## Template file syntax

### Expressions
**$(**`c++ expression`**)**  
Expressions are used to add application's data to the template. It can be any valid C++ expression, with only condition, that its result must be streamable to the default output stream `std::ostream`. Expressions can be placed anywhere in the HTML template, besides the tag names.
In our todolist example, `$(cfg.name)` is an expression adding template config variable `name` to the result page.

### Statements
**${**`c++ statement(s)`**}**  
Statements are used to add any valid C++ code to the template rendering function. For example, you can add variables, classes declarations or lambdas.  Let's say that you don't like the default name `cfg` used for passing data to the template. You can create a reference to it with any name you like and use it later:

```html
    ${ auto& param = cfg;}
    <h1>$(param.name)'s todo list:</h1>
```

Note, that `cfg` and `out` are reserved names used for parameters in generated
rendering function. Also, don't put anything in the `htcpp` namespace.

### Global statements
**#{**`c++ statement(s)`**}**  
These statements are used to add any valid C++ code outside the template rendering function. Unlike regular statements, with global ones you can add include directives or functions definitions. Global statements can only be placed in the top level of `htcpp` template, outside any HTML element. Please, don't put anything in the `htcpp` namespace.


### Control flow extensions
If **hypertextcpp** used a common approach for control flow in HTML template engines, our todolist example would look something like this:
```html
<html>	
    <body>
        <h1>%%cfg.name%%'s todo list:</h1>
        %%if cfg.tasks.empty()%%
        <p> No tasks found </p>				
        %%end%%
        <ul>
            %%for auto task : cfg.tasks%%
                <li>$(task.name)</li>
            %%end%%
        </ul>        
    </body>
</html>

```

In our opinion it significantly hurts readability of document tree  and makes it hard to choose indentation and keep it consistent - notice how  different approaches are used for `if` and `for` blocks in the example.

**hypertextcpp** solves this problem (*and is created mainly for solving this problem*) by  applying control flow to the HTML elements itself without  adding logic block scopes to the document. It uses just two extensions for tags to make this work:

* Conditional extension  
  **?(**`c++ condition`**)**  
  HTML elements with this extension are added to the document only when condition is fulfilled.
  Example of usage from our todolist template:
  ```html
      <p> No tasks found </p>?(cfg.tasks.empty())
  ```

* Loop extension  
  **@(**`c++ init-statement; condition; iteration_expression`**)**  
  or  
  **@(**`c++ range_declaration : range_expression`**)**  
  HTML elements with this extension are added to the document multiple times, on each step of the loop, or for each element of the iterated range.
  Example of usage from our todolist template:
  ```html
      <li>$(task.name)</li>@(auto task : cfg.tasks)
  ```
  Lets add another example for other type of `for` loop:
  ```html
      <li>Task#$(i)</li>@(auto i = 0 ; i < 3; ++i)
  ```
  which evaluates to
  ```html
      <li>Task#0</li>
      <li>Task#1</li>
      <li>Task#2</li>
  ```

Both extensions can be added to the opening or closing tag, but each tag and HTML element can only have one extension.

It's recommended to add extension to the opening tag for the multiline HTML elements:
```html
<div>?(cfg.greet)
    <p>Hello world!</p>
</div>
```
and to the closing tag for the single-line ones:
```html
<div><p>Hello world!</p></div>?(cfg.greet)
```

Note, that while extensions hide control flow block scopes from the template document, they're still present in the generated C++ code and implemented with regular `if` and `for` control structures. Therefore, template like this:

```html
    <div>@(auto i = 0; i<3; ++i)
        ${ auto num = i*2;}
        <p> Item #$(num)<p>
    </div>
    <p> Last num is $(num) </p>
```
won't compile because the `num` variable isn't visible outside the `for` block scope generated by loop extension on the `div` tag.

### Sections
**\[\[** `text, html elements, statements, expressions or other sections` **]]**  
Sections can contain a part of template document, and it's possible to attach control flow extensions to them. Their main usage is adding attributes to HTML elements conditionally.

Let's update todolist example by adding a line-through text style to completed tasks:  
[`examples/02/todolist.htcpp`](examples/02/todolist.htcpp)
```html
<html>	
    <body>
        <h1>$(cfg.name)'s todo list:</h1>
        <p> No tasks found </p>?(cfg.tasks.empty())
        <ul>
            <li [[style="text-decoration: line-through;"]]?(task.isCompleted)>$(task.name)</li>@(auto task : cfg.tasks)
        </ul>
    </body>
</html>
```

Don't forget to update the C++ template config structure!  
[`examples/02/todolist_printer.cpp`](examples/02/todolist_printer.cpp)
```cpp
//...
struct PageParams{
        struct Task{
            std::string name;
            bool isCompleted = false;
        };
        std::string name = "Bob";
        std::vector<Task> tasks = {{"laundry", true}, {"cooking", false}};
    } pageParams;
//...
```

Tip of the day: Keep your template tidy and don't introduce sections when it's possible to attach extensions to HTML elements.


### Procedures and partial rendering
**#**`procedureName`**(){**`html elements, statements, expressions or sections`**}**

Parts of `htccp` template can be placed inside procedures - parameterless functions capturing the `cfg` variable. They are available for call from the C++ application, so if any part of the page needs to be rendered separately from the whole template, procedures are a big help.
Procedures can only be placed in the top level of `htcpp` template, outside any HTML element.
Let's put the list of tasks from the todolist example in the procedure:  
[`examples/03/todolist.htcpp`](examples/03/todolist.htcpp)
```html
#taskList(){
    <li [[style="text-decoration: line-through;"]]?(task.isCompleted)>$(task.name)</li>@(auto task : cfg.tasks)
}
<html>	
    <body>
        <h1>$(cfg.name)'s todo list:</h1>
        <p> No tasks found </p>?(cfg.tasks.empty())
        <ul>
            $(taskList())
        </ul>
    </body>
</html>
```

Now the tasks list can be output to stdout by itself like that:  
[`examples/03/todolist_printer.cpp`](examples/03/todolist_printer.cpp)
```cpp
    //...
    auto page = todolist{};
    page.print("taskList", pageParams); 
    //...
```

## Code generation
### Command line parameters
 ```console
kamchatka-volcano@home:~$ hypertextcpp --help
Usage: hypertextcpp <input> [params] [flags] 
Arguments:
    <input> (path)               .htcpp file to transpile
Parameters:
   -o, --output <path>           output c++ file path
                                   (if empty, current working directory is used)
                                   (optional, default: "")
   -c, --class-name <string>     generated class name
                                   (optional)
Flags:
   -s, --shared-lib              generate result as shared library source 
                                   files
       --class-pascalcase        generate class name by using .htcpp filename 
                                   in PascalCase
       --class-snakecase         generate class name by using .htcpp filename 
                                   in snake_case
       --class-lowercase         generate class name by using .htcpp filename 
                                   in lowercase
       --help                    show usage info and exit


Process finished with exit code 0

 ```

### Single header renderer
By default, the **hypertextcpp** transpiler works in a single header mode and generates a C++ header file that you're supposed to simply include in your project. A generated renderer class has a name of `.htcpp` template file. You can override the name by using `--class-name` parameter, or you can specify one of the following flags `--class-pascalcase`, `--class-snakecase` or `--class-lowercase` to use `.htcpp` template's filename converted to the corresponding case as a class name.
Converting template to C++ code each time you modify it is a laborious task, so it makes sense to add this step to your build process. Let's demonstrate on how to do it with CMake and at the same time rename the renderer class.

[`examples/04/CMakeLists.txt`](examples/04/CMakeLists.txt)
```
cmake_minimum_required(VERSION 3.13)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_custom_command(
    OUTPUT  todolist.h
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp -o ${CMAKE_CURRENT_SOURCE_DIR}/todolist.h --class-name TodoList
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp
)

set(SRC
    main.cpp
    todolist.h)

add_executable(todolist_printer ${SRC})
```

Now, everytime you change the template, the corresponding header will be regenerated on the next build.

### Shared library renderer
It can feel quite wasteful to rebuild your project each time the template file is changed, so **hypertextcpp** supports generation of C++ source file for building templates in form of shared libraries and linking them dynamically from your application.
It requires to duplicate the config declaration in `.htccp` template, registering it with `HTCPP_CONFIG` macro both in template and in the application source, generate the renderer code with  `--shared-lib` command line flag, build the library and load it using the tiny API installed from the `shared_lib_api/` directory. It sounds scarier than it is, so let's quickly update the todolist example to see how it works.

First we need to copy the config structure declaration in the template:  
[`examples/05/todolist.htcpp`](examples/05/todolist.htcpp)
```html
#{
    #include <vector>
    struct PageParams{
        struct Task{
            std::string name;
            bool isCompleted = false;
        };
        std::string name = "Bob";
        std::vector<Task> tasks = {{"laundry", true}, {"cooking", false}};
    } pageParams;
    HTCPP_CONFIG(PageParams);
}

#taskList(){
    <li [[style="text-decoration: line-through;"]]?(task.isCompleted)>$(task.name)</li>@(auto task : cfg.tasks)
}
<html>	
    <body>
        <h1>$(cfg.name)'s todo list:</h1>
        <p> No tasks found </p>?(cfg.tasks.empty())
        <ul>
            $(taskList())
        </ul>
    </body>
</html>
```

Be sure to use an exact copy, any mismatch of the config structure between template and application can't be handled gracefully, so if you try to load a template library with different structure you'll definitely crash the application and maybe hurt someone as a result.

Next, we need to build our template renderer as a library. It's not possible to bundle multiple templates file in one library, so we can easily use a generic CMake file, that builds a library from a single `.htcpp` file:  
[`shared_template/CMakeLists.txt`](shared_template/CMakeLists.txt)
```
cmake_minimum_required(VERSION 3.13)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(NAME "Template name")

add_custom_command(
    OUTPUT  ${NAME}.cpp
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.htcpp -o ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp -s
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.htcpp
)

add_library(${NAME} SHARED ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp)
```

Copy `todolist.htcpp` in  `shared_template/` and execute the following commands:
```console
kamchatka-volcano@home:~/shared_template$ mkdir build; cd build; 
kamchatka-volcano@home:~/shared_template/build$ cmake -DNAME=todolist ..; make
```
If everything goes right, you'll get the `libtodolist.so` in the build directory. That's our `todolist.htcpp` template compiled as the shared library.  
Next, let's modify the `todolist_printer.cpp` to be able to load it:  
[`examples/05/todolist_printer.cpp`](examples/05/todolist_printer.cpp)
```cpp
#include <hypertextcpp/templateloader.h>
#include <string>
#include <vector>

struct PageParams{
struct Task{
std::string name;
bool isCompleted = false;
};
std::string name = "Bob";
std::vector<Task> tasks = {{"laundry", true}, {"cooking", false}};
};
HTCPP_CONFIG(PageParams);

int main()
{
auto pageParams = PageParams{};
auto page = htcpp::loadTemplate<PageParams>("libtodolist.so");
page->print(pageParams);
return 0;
}
```

Don't forget to add linking of system  library `dl` to the build config - that allows us to link the template library dynamically.  
`todolist_printer` should compile and work the same as the previous example using the single header approach.

## Installation

```console
git clone https://github.com/kamchatka-volcano/hypertextcpp.git
cd hypertextcpp && mkdir build && cd build
cmake ..
make install
```

## Running tests
```console
cd hypertextcpp/build
cmake .. -DENABLE_TESTS=ON
make
ctest
```

## License
**hypertextcpp** is licensed under the [MS-PL license](/LICENSE.md)  
The bundled part of the [GSL](https://github.com/microsoft/GSL) library is licensed under the [MIT license](https://github.com/microsoft/GSL/blob/main/LICENSE)
