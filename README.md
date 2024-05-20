<center>
    <h1>enumerable::</h1>
    <p>A lightweight header-only library that implements reflective enums for C++</p>
</center>

## About the library

### Advantages
- Header-only
- Supports string <-> enum conversion
- Fast
- Supports std::cout by default
- Can be used inside classes
- Small binary footprint
- Supports C++11 or higher

### Limitations
- Doesn't support unscoped enums
- Doesn't support aliased enum fields
    - **Note:** using it *is not forbidden on itself*, however, converting the value to an string on will not return the alias name! (eg.: given `foo = 0, bar = 0`, `enums::to_string(bar)`  will return `"foo"`, and not `"bar"`)
- Inheriting the `enums::enumerable<T>` class *is mandatory*
- Processing is done at program startup

## Quick usage:

To create a reflective enum, replace this:

```cpp
enum class fruit {
    apple      = 0, 
    banana     = 1, 
    pineapple  = 2, 
    watermelon = 3
};
```

... with this:


```cpp
enumerable(fruit, 
    apple      = 0, 
    banana     = 1, 
    pineapple  = 2, 
    watermelon = 3
);
```

## Examples:

### Iterating over enums by key/value

```cpp
#include <iostream>
#include <enumerable.h>

enumerable(fruit, apple = 0, banana, pineapple, watermelon);

// ...

// Key iteration
for (const auto i : fruit::names()) {
    std::cout << i << std::endl;
}

// Value iteration
for (const auto i : fruit::values()) {
    // NOTE: by default, the values() function is going to
    //       return an array of integers, so you'll have
    //       to convert it manually
    std::cout << fruit(i) << std::endl;
}

// Key/value iteration
for (const auto i : fruit::items()) {
    std::cout << i.first // Name
              << " = "
              << i.second // Value
              << std::endl;
}
```

### Conversion and `std::` integration

```cpp
#include <iostream>
#include <enumerable.h>

decl_enum(size, small, medium, large);

// ...

// Explicit conversion, prints "medium"
std::cout << color(size::medium).string() << std::endl;

// Implicit conversion, prints "size::medium"
std::cout << color(size::medium) << std::endl;

// Global string conversion, prints "small"
std::cout << enums::to_string(size::small) << std::endl;
```

## Todo:
- [ ] Improve support for `std::` stuff
- [ ] Improve compile-times
- [ ] Improve modularity
- [x] ~I'm going insane!~

## License:

This library is released to the public domain, the usage of this library does not require any credit, and allows further modification with no restrictions.

However, if you use it on a project and wish for it to be added here, please let me know! I would love to see what you guys are creating with it :D
