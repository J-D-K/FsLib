# FsLib code style guidelines
**Note:** I wrote these as I went. They are in no particular order.

1. Braces should always be on their own line with one exception: When being used to declare a function in a class that has no code. Ex:
```cpp
if(expression)
{
    // Code.
}

class Derived : public Base
{
    public:
        // In this case, it prevents two unneeded lines. This is acceptable and more readable.
        Derived() : Base(variable) {};
};
```

All blocks and statements **should** include braces. Even if there is only one line of code executed after the statement is evaluated.

2. Namespaces should always be lowercase:
```cpp
namespace fslib
{

}
```

3. Classes and types: Upper camel case. Ex:
```cpp
class ClassName
{
    public:
        ClassName() = default;
    
    private:
        // Member variables.
};

typedef struct
{
    int data;
} StructName;

typedef enum
{
    Type1,
    Type2,
    Type3
} EnumName;
```
4. Member functions and methods: Lower snake case. Ex:
```cpp
class ClassName
{
    public:
        ClassName() = default;

        void member_function_name();
};
```

5. Member methods should be called prefixed with the class they belong to to avoid confusion when inheritance and polymorphism is involved. Ex:
```cpp
class BaseClass
{
    public:
        void member_method();
}

class DerivedClass : public BaseClass
{
    public:
        void member_method_b()
        {
            BaseClass::member_method();
        }
}
```

6. Class member variables: Prefixed by either `m_` for members, or `sm_` for static members followed by lower camel case for the variable name. Ex
```cpp
class ClassName
{
    public:
        ClassName() = default;

    private:
        int m_classNameVariable = 0;
        static inline int sm_staticSharedVariable = 0;
};
```

7. Functions: Lower snake case, prefixed with the namespace they belong to. No `namespace x{` in source files. Ex:
```cpp
void fslib::function_name()
{
    // Do stuff here.
}
```

8. Function arguments: Lower camel case. Ex:
```cpp
void fslib::function_name(int argName)
```

9. Local variables: Lower camel case. Ex:
```cpp
int numberHere = 0;
```

10. Global variables should be avoided at all costs. If it can't be, they should be prefixed with `g_` followed by lower camel case name. Ex:
```cpp
int g_globalVariableName = 0;
```
11. Static and anonymous namespace variables should be prefixed by `s_` followed by lower camel case. Ex:
```cpp
static int s_variableName = 10;

namespace
{
    constexpr int s_variableName = 10;
}
```
12. C style macros and all constants should be all capital with underscores. C style macros should be avoided and used sparingly unless they can drastically clean up code. Ex:
```cpp
#define MACRO_NAME(x)

static constexpr int CONST_VALUE_NAME = 10;

namespace
{
    constexpr int CONST_VALUE_NAME = 10;
}
```
