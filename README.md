# dr_param
This library contains utilities for reading complex structs from YAML files.
It also includes functions to perform simple preprocessing on YAML files.
The preprocessing adds support for including other YAML files and parameter expansion in strings.

The library uses `yamlcpp` for the `YAML::Node` type, so it's fully compatible with `yamlcpp`.
However, some functions from `yamlcpp` have alternatives here with extended functionality and/or better error reporting.
When available, you should prefer using functions from this library over functions from `yamlcpp`.

For historical reasons, it still includes utilities for loading information from the ROS parameter server too, but these are deprecated.
Using the ROS parameter server is considered harmful and should be avoided.
New code should always read configuration parameters from the command line and/or a configuration file.

# Reading YAML files.
Reading a YAML file is easy.
We can simply call `dr::readYamlFile` from `yaml.hpp`.
This is almost identical to `YAML::LoadFile`, except that it returns an `estd::result` with the `YAML::Node`.
That way, more error details are preserved when reading or parsing the file fails.

It's also possible to perform some pre-processing when loading the file.
For that, we can use `dr::preprocessYamlFile` and friends from `yaml_preprocess.hpp`.
The preprocessing adds two two features to the YAML tree:
it allows you to include other YAML files,
and it can perform parameter expansion in string values.

Refer to the documentation for `dr::preprocessYamlFile` for more details.
Some examples of YAML files with preprocessing directives can be found in the `test/data` folder of this library.

# Using YAML conversions.

The main purpose of this library is to perform conversion to/from YAML nodes.
The main interface to perform these conversions are the functions `parseYaml<T>(node)` and `encodeYaml(value)`.

For example, parsing a configuration file:
```cpp
YAML::Node yaml = dr::readYamlFile("config.yaml").value();
auto config = dr::parseYaml<Config>(yaml).value();
```

Or converting back to a YAML node:
```cpp
Config config = {...};
YAML::Node yaml = dr::encodeYaml(config);
```

Note that the called functions in these examples return `estd::result` objects.
To keep these examples simple and to the point, they simply call `.value()` on the result objects.
That converts the `result` to the raw value or throws an exception.
For real use cases, you should usually handle errors differently.

# Defining new YAML conversions.

Conversions to/from YAML use `estd::convert` behind the scenes.
This means that conversions are implemented as a specilization of the struct `estd::conversion<From, To>`.
For conversions from `T` to YAML nodes, this is simply `estd::conversion<T, YAML::Node>`.

Conversions from YAML nodes can fail and must return a `dr::YamlResult<T>`.
As such, the struct specialization is `estd::conversion<YAML::Node, dr::YamlResult<T>>`.
Note that `YamlResult<T>` is simply an alias for `estd::result<T, dr::YamlError>`.

The actual conversion is implemented as the static member function `To perform(From const & value)`.
For partially specialized conversions, you can also define a `static constexpr bool possible = ...` to disable the conversion in a SFINAE friendly manner.

As an example, these could be the conversions for a simple struct:

```cpp
/// The struct that we'll be converting.
struct Foo {
  int a;
  float b;
};

// The conversion to YAML node.
template<>
struct estd::conversion<Foo, YAML::Node> {
  static YAML::Node perform(Foo const & value) {
    YAML::Node result;
    result["a"] = dr::encodeYaml(value.a);
    result["b"] = dr::encodeYaml(value.b);
    return result;
  }
};

/// The conversion from YAML node.
template<>
struct estd::conversion<YAML::Node, dr::YamlResult<Foo>> {
  static dr::YamlResult<Foo> perform(YAML::Node const & node) {
    // Assert that the YAML node is a map of size 2.
    auto error = dr::expectMap(node, 2);

    int a = dr::parseYaml<int>(node["a"]);
    if (!a) return a.error().appendTrace({"a", "int", node["a"].Type()});

    int b = dr::parseYaml<float>(node["b"]);
    if (!b) return a.error().appendTrace({"b", "float", node["b"].Type()});

    return Foo{a, b};
  }
};
```

The `error().appendTrace(...)` bit may seem slightly frightening.
It is there to be able to provide high-quality error messages to the user when parsing fails.
The purpose of `appendTrace()` is to record the full path to the YAML node in the tree that caused the error,
including the name, the expected type and the raw YAML type.

There are some issues with this implementation though.
Firstly, if you have an unrecognized key in the YAML, the error message will not refer to the unknown key.
Instead, it will tell you that it's missing a key with the correct spelling.
A nuisance, but not the end of the world.

More importantly though, it's quite a bit of work to implement this by hand.
Especially when the struct is large, writing this code is tedious en error-prone.
The potential for spelling mistakes and copy-paste errors would be very high.

To alleviate this, these functions can be generated from a simpler definition: struct decompositions.
Read on to find out how.

# Struct decompositions.

To automatically generate YAML conversions for structs or classes,
we must provide some compile-time information about the members of the type.
In this library, these are called decompositions: they decompose a type into it's members.
Here, a type that can be decomposed is also called a decomposable type.

The nitty-gritty details can be found in the `decompose.hpp` header,
but here we'll stick with the somewhat easier `decompose_macros.hpp`.
These decompositions could be used for other purposes as well,
but we're mainly interested in using it to generate the YAML conversions for us.

Basically, all we need to do is define a struct decomposition and include the right header.
If we do that, we already get automaticly generated YAML conversions:

``cpp
// This header contains the convenience macros to define struct decompositions.
#include <dr_param/decompose_macros.hpp>

// This header enables automatic YAML conversions for decomposable types.
#include <dr_param/yaml_decompose.hpp>

struct Bar {
  float baz;
};

struct Foo {
  int a;
  Bar b;
};

// Here we define the decomposition using the convenience macro for the struct Bar.
// Note: this macro *must* be invoked from the global namespace.
DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(Bar,
  (bar, "float", "The baz member of Bar.")
)

// The same for Foo.
DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(Foo,
  (a, "int",   "The a member of Foo")  // No comma here!
  (b, "Bar",   "The b member of Foo")
)

void foo(YAML::Node const & root) {
  Foo foo = dr::parseYaml<Foo>(root).value();

  // Great success!
}
```

One important thing things to keep in mind: **the automatic YAML conversion for a decomposable struct can only work if all members are convertible**.
For members that are structs themselves, the conversion could also be the automatic conversion generated from the struct decomposition.
So, as long as a struct can be recursively decomposed into types with explicit YAML conversions, the automatic conversion is possible.

# Reducing compile times.

The automatic conversions from the `yaml_decompose.hpp` header are easy to use, but they can incur a reasonable compile time penalty.
This can be avoided though, the same way that you would usually reduce compile time:
put a declaration in a header file, and define the implementation in a source file.
For re-used libraries, it is probably a good idea to do this.

However, we don't want to be back to square one having to write the whole conversion by hand.
Fortunately, we don't have to.
We can still delegate to the automatic conversion generated from the struct decomposition.
We'll just have to move things around a bit.

There are also some helper macros in `yaml_macros.hpp` to make it easier to define and declare YAML conversion.
These aren't tied to struct decompositions, but they'll still help us write less code.

For example let's reimplement the example from the previous section,
but this time we'll split the declarations and definitions.

We start with the header container the declarations:
```cpp
// This header defines macros to easily declare and define YAML conversions.
#include <dr_param/yaml_macros.hpp>

// Note: we don't include any decomposition headers here.
// That would just pollute the namespace uneccesarily.

struct Bar {
  float baz;
};

struct Foo {
  int a;
  Bar b;
};

// Now we use the macros to *delcare* (but not define) the YAML conversions.
// Note: like the decomposition macros, these *must* be called from the global namespace.
DR_PARAM_DECLARE_YAML_CONVERSION(Bar);
DR_PARAM_DECLARE_YAML_CONVERSION(Foo);
```

That's the header done.
The header is enough to make the compiler happy, so we can already use the conversions elsewhere as usual:

```cpp
#include <foolib/header.hpp>

void foo(YAML::Node const & root) {
  Foo foo = dr::parseYaml<Foo>(root).value();

  // Great success!
}
```

While the compiler is happy, the linker is not.
If we accidentally forget about the definitions in a  source file, any code using the conversion would compile fine, but the linking stage would fail.
This is exactly what you would expect when a declaration is available without a matching definition.
However, we prefer our code to link fine as well, so let's also look at the matching source file.

```cpp
// Include the header as usual.
#include "header.hpp"

// Now we include decompose_macros.gpp and yaml_decompose.hpp.
// This time we can polute the namespace just fine,
// because the effects are limited to this source file.
#include <dr_param/decompose_macros.hpp>
#include <dr_param/yaml_decompose.hpp>

// Just including the header isn't enough though.
// We must explicitly define the conversions to match the declarations from the header.
// Fortunately, we have helper macros for that too.

// For the actual implementation, we'll delegate to the struct decomposition.
// We have to define those first though.

DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(Bar,
  (bar, "float", "The baz member of Bar.")
)

// The same for Foo.
DR_PARAM_DEFINE_STRUCT_DECOMPOSITION(Foo,
  (a, "int",   "The a member of Foo")  // No comma here!
  (b, "Bar",   "The b member of Foo")
)

// Now we can use the helper macros and functions from yaml_decompose.hpp
// to define the conversions.

DR_PARAM_DEFINE_YAML_DECODE(Bar, node) {
  return dr::parseDecomposableFromYaml<Bar>(node);
}

DR_PARAM_DEFINE_YAML_ENCODE(Bar, value) {
  return dr::encodeDecomposableAsYaml(node);
}
```

To sum it up, we can reduce compile time by *declaring* YAML conversion in a header.
That header no longer needs to define a struct decomposition and it shouldn't include `yaml_decompose.hpp`.
Instead, we'll put the struct decompositions in the source file, together with the *definitions* of the YAML conversions.
Those definitions can then simply delegate to the conversions generated from the struct decomposition.
