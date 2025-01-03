<p align="center">
  <img height="96" src="doc/logo.png"/>  
</p>

**hypertextcpp** is an HTML templating system for C++ applications.
It provides an [.htcpp](https://marketplace.visualstudio.com/items?itemName=kamchatka-volcano.htcpp) template file format  and a command-line utility that transpiles it to C++ HTML rendering code. Include the generated C++ header file in your project, set up a build system to update it when necessary, and you're all set.

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
    * [Header and source renderer](#header-and-source-renderer)
    * [Shared library renderer](#shared-library-renderer)
* [Installation](#installation)
* [Running tests](#running-tests)
* [License](#license)

## Template file syntax

### Expressions
**$(**`c++ expression`**)**
Expressions are used to add the application's data to the template. It can be any valid C++ expression, with the only condition that its result must be streamable to the default output stream `std::ostream`. Expressions can be placed anywhere in the HTML template, except in tag names.
In our todolist example, `$(cfg.name)` is an expression that adds the template config variable `name` to the result page.

### Statements
**${**`c++ statement(s)`**}**
Statements are used to add any valid C++ code to the template rendering function. For example, you can add variables, class declarations, or lambdas. Let's say you don't like the default name `cfg` used for passing data to the template. You can create a reference to it with any name you like and use it later:

```html
    ${ auto& param = cfg;}
    <h1>$(param.name)'s todo list:</h1>
```

Note, that `cfg` and `out` are reserved names used for parameters in the generated
rendering function. Also, don't put anything in the `htcpp` namespace.

### Global statements
**#{**`c++ statement(s)`**}**  
These statements are used to add any valid C++ code outside the template rendering function. Unlike regular statements, with global ones you can add include directives or function definitions. Global statements can only be placed at the top level of the `htcpp` template, outside any HTML element. Please don't put anything in the `htcpp` namespace.

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

In our opinion, it significantly hurts the readability of the document tree and makes it hard to choose indentation and keep it consistent — notice how different approaches are used for if and for blocks in the example.

**hypertextcpp** solves this problem by applying control flow to the HTML elements themselves without adding logic block scopes to the document. It uses just two extensions for tags to make this work:

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
  HTML elements with this extension are added to the document multiple times, on each step of the loop or for each element of the iterated range.
  Example of usage from our todolist template:
  ```html
      <li>$(task.name)</li>@(auto task : cfg.tasks)
  ```
  Let's add another example for another type of `for` loop:
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

It's recommended to add the extension to the opening tag for multiline HTML elements:
```html
<div>?(cfg.greet)
    <p>Hello world!</p>
</div>
```
and to the closing tag for the single-line ones:
```html
<div><p>Hello world!</p></div>?(cfg.greet)
```

Note that while extensions hide control flow block scopes from the template document, they're still present in the generated C++ code and implemented with regular `if` and `for` control structures. Therefore, a template like this:

```html
    <div>@(auto i = 0; i<3; ++i)
        ${ auto num = i*2;}
        <p> Item #$(num)<p>
    </div>
    <p> Last num is $(num) </p>
```
won't compile because the `num` variable isn't visible outside the `for` block scope generated by the loop extension on the `div` tag.

### Sections
**\[\[** `text, html elements, statements, expressions or other sections` **]]**  
Sections can contain a part of the template document, and it's possible to attach control flow extensions to them. Their main usage is adding attributes to HTML elements conditionally.

Let's update the todolist example by adding a line-through text style to completed tasks:
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

Parts of the `htcpp` template can be placed inside procedures—parameterless functions capturing the `cfg` variable. They are available for call from the C++ application, so if any part of the page needs to be rendered separately from the whole template, procedures are a big help.
Procedures can only be placed at the top level of the `htcpp` template, outside any HTML element.
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

Now the tasks list can be output to stdout by itself like this:  
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
Usage: hypertextcpp [commands] [flags] 
Flags:
  --help                                      show usage info and exit
Commands:
    generateHeaderOnly [options]              generate header only file
    generateSharedLibrarySource [options]     generate shared library source 
                                                file
    generateHeaderAndSource [options]         generate header and source files
 ```

Single header renderer generation:
 ```console
kamchatka-volcano@home:~$ hypertextcpp generateHeaderOnly --help
Usage: hypertextcpp generateHeaderOnly <input> [params] [flags] 
Arguments:
    <input> (path)         .htcpp file to transpile
   -outputDir=<path>       output dir
                             (if empty, current working directory is used)
                             (optional, default: "")
   -className=<string>     generated class name
                             (if empty, input file name is used)
                             (optional, default: "")
Flags:
  --help                   show usage info and exit
 ```

Header and source renderer generation:
 ```console
kamchatka-volcano@home:~$ hypertextcpp generateHeaderAndSource --help
Usage: hypertextcpp generateHeaderAndSource <input> -configClassName=<string> [params] [flags] 
Arguments:
    <input> (path)               .htcpp file to transpile
Parameters:
   -configClassName=<string>     config class name
   -outputDir=<path>             output dir
                                   (if empty, current working directory is used)
                                   (optional, default: "")
   -className=<string>           generated class name
                                   (if empty, input file name is used)
                                   (optional, default: "")
Flags:
  --help                         show usage info and exit
 ```

Shared library renderer generation:
 ```console
kamchatka-volcano@home:~$ hypertextcpp generateSharedLibrarySource --help
Usage: hypertextcpp generateSharedLibrarySource <input> [params] [flags] 
Arguments:
    <input> (path)       .htcpp file to transpile
   -outputDir=<path>     output dir
                           (if empty, current working directory is used)
                           (optional, default: "")
Flags:
  --help                 show usage info and exit
 ```


### Single header renderer
In this mode, the **hypertextcpp** generates a C++ header file that you're supposed to simply include in your project. A generated renderer class has the name of the `.htcpp` template file.
Converting the template to C++ code each time you modify it is a laborious task, so it makes sense to add this step to your build process. To do this with CMake you can use `hypertextcpp_GenerateHeader` function from the `hypertextcpp.cmake` file.
Note that this function launches the `hypertextcpp` executable, so it should be installed on your system first. 

[`examples/04/CMakeLists.txt`](examples/04/CMakeLists.txt)
```
cmake_minimum_required(VERSION 3.18)


include(../../hypertextcpp.cmake)
hypertextcpp_GenerateHeader(
  TEMPLATE_FILE todolist.htcpp 
  CLASS_NAME TodoList
)

set(SRC
    todolist_printer.cpp
    todolist.h)

#Please note that to generate the header todolist.h, it must pe passed to the target sources    
add_executable(todolist_printer ${SRC})

target_compile_features(todolist_printer PUBLIC cxx_std_17)
set_target_properties(todolist_printer PROPERTIES CXX_EXTENSIONS OFF)
```

Now, every time you change the template `todolist.htcpp`, the corresponding header will be regenerated on the next build.

### Header and source renderer
It can feel quite wasteful to rebuild all object files that include the renderer header each time the template file is changed, so `hypertextcpp` supports the generation of the renderer as a header and implementation file. In this mode, the generated rendering methods aren't function templates, and you need to provide the config name as a command-line parameter and either define the config structure inside the template or include it from there (check `examples/ex_06` and `examples/ex_07`).
To do this with CMake, you can use the `hypertextcpp_GenerateHeaderAndSource` function from the `hypertextcpp.cmake` file.

```
hypertextcpp_GenerateHeaderAndSource(
        TEMPLATE_FILE todolist.htcpp
        CONFIG_CLASS_NAME PageParams)
```

### Shared library renderer
 **hypertextcpp** also supports the generation of a C++ source file for building templates in the form of shared libraries and linking them dynamically from your application. This way it's possible to rebuild the template and update it without restarting the application.
It requires duplicating the config declaration in the .htcpp template, registering it with the `HTCPP_CONFIG` macro in both the template and the application source, generating the renderer code with the `generateSharedLibrarySource` command, building the library, and loading it using the tiny API installed from the `shared_lib_api/` directory. It sounds scarier than it is, so let's quickly update the todolist example to see how it works.

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

Be sure to use an exact copy; any mismatch in the config structure between the template and the application can't be handled gracefully. So, if you try to load a template library with a different structure, your application will abort with a runtime error. This means that by using a htcpp template in the form of shared libraries, you lose one of the main advantages of hypertextcpp—compile-time type safety. Because of this, it's recommended to use this mode only after your template config has stabilized and doesn't change often.

Next, we need to build our template renderer as a library. It's not possible to bundle multiple template files in one library, so we can build a library from a single `.htcpp` file by using the `hypertextcpp_BuildSharedLibrary` CMake function from `hypertextcpp.cmake`:

```
cmake_minimum_required(VERSION 3.18)

hypertextcpp_BuildSharedLibrary(
        TEMPLATE_FILE todolist.htcpp
)
add_dependencies(${PROJECT_NAME} todolist)
```

If everything goes right, you'll get `libtodolist.so` in the build directory. That's our `todolist.htcpp` template compiled as a shared library.
Next, let's modify `todolist_printer.cpp` to be able to load it:
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

On Linux, it may be necessary to link the system library `dl` to the build config, as we load the template library dynamically.
`todolist_printer` should compile and work the same as the previous example using the single-header approach.

## Installation

```console
git clone https://github.com/kamchatka-volcano/hypertextcpp.git
cd hypertextcpp
cmake -S . -B build
cmake --build build
cmake --install build
cmake --install build --component shared_lib_api
```
You can skip the installation of `shared_lib_api` component if you don't need to load templates in shared libraries form. 

## Running tests
```console
cd hypertextcpp
cmake -S . -B build -DENABLE_TESTS=ON
cmake --build build 
cd build/tests && ctest
```

## License
**hypertextcpp** is licensed under the [MS-PL license](/LICENSE.md)
