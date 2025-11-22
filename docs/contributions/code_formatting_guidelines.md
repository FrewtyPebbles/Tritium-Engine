# Code Formatting Guidelines

These guidelines are strictly enforced to maintain code readability as the project scales.  There will be times where it makes sense to break one of the rules in the guideline, that is okay.

## Naming Conventions

### Local Variables

camelCase

 > If the variable is only used for or is of a type belonging to some 3rd party
 library like SDL2 or Vulkan, you should give your
 variable a prefix like `sdl_variableName` or `vk_variableName`.

---

### Constants

CONSTANT_CASE

---

### Class Names

TitleCase

---

### NameSpace Names

TitleCase

---

### Class Attributes

snake_case

---

### Function Arguments

snake_case

 > If the argument is only used for or is of a type belonging to some 3rd party
 library like SDL2 or Vulkan, you should give your
 argument a prefix like `sdl_argument_name` or `vk_argument_name` so long as the function name
 does not include the prefix already.  Also just do what makes sense when following this guideline.

---

### Function Names

snake_case

 > If you need to include the name of a 3rd party library in a function name,
 then try to include it in a prefix before the name like `vk_function_name` or `SDL_function_name`.

---

## Code Structure

### Code Block Brackets

The opening bracket for a code block should be on the same line as the statement it is related to.

This is correct:

```cpp
void my_function() {

}
```

This is incorrect:

```cpp
void my_function()
{

}
```

This applies to everything: statements, class definitions, namespaces, functions, etc.