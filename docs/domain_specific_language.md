# About Domain Specific Language

The game engine will (in the future) allow a domain specific language as an alternative to C++ and HLSL.
This language will look similar to python, but be statically typed, not reference counted, and flow more like C#.

This document defines some of the syntax for the language.

## Language Spec

 - Every type is heap allocated by default unless the `temp` keyword is provided.

 - This language uses a garbage collector.

 - Errors are handled as values.

 - A altered subset of this language will be used for writing shaders.

## Variable Declarations

```
# This is a comment

# This is a heap allocated list of integers
variable_name:list[int] = []

# This is a stack allocated list of integers
temp variable_name:list[int] = []
```

## Classes and Functions

Classes will most likely only support single inheritance since they will define the behavior of nodes.

```
# This defines the current file/script as a class
class ScriptClassName extends BaseClassName

static fn method_name(arg:int):
	???
		This is a script class static method and can be called via ScriptClassName.method_name(arg)
	???
	pass

fn method_name2(self, arg:int):
	???
		This is a script class method and can be called via script_class_instance.method_name2(arg)
	???
	pass

# This is a locally defined class
class LocalClassName extends BaseClassName:
	
	fn op create(self, arg1:int, arg2:map[str, list[bool]]):
		???
			"op" functions are like operators for the class, this would be the initialization function.

			Also this is a docstring.  if you need to put 3 questionmarks in a docstring you can do \???
		???
		pass

	fn op delete(self):
		pass

	fn method(self, something:ScriptClassName):
		pass

	fn op close(self):
		???
			This is called at the end of a "with" statement.
		???
```

## Operators

```
variable_or_instance as int
# this is a typecast

1 + 1

1 - 1

1 * 1

1 / 1

1 % 1

3.0 // 1
# this is floor division

1 ^ 2
# this is a pow(1, 2) operation

-1

not true

1 == 1

1 != 1

1 > 1

1 >= 1

1 <= 1

true and false

false or true

```

## Loops and Statements

```

if some_condition:
	pass
elif some_other_condition:
	pass
else:
	pass

for thing in collection:
	pass

for i, thing in enumerate(collection):
	pass

while condition:
	pass

match:
	condition:
		pass
	default:
		pass

with closeable_object as closeable_object_reference:
	# This makes a reference of closeable_object called closeable_object_reference
	# it closes it at the end of the with statement by calling `closeable_object_reference.close()`
	pass

```