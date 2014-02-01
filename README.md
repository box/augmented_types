# Augmented Types

Augmented Types is a PHP extension that enforces PHPDoc-style type annotations at runtime. It is compatible with opcode cachers and allows for selective enforcement of function type annotations, enabling incremental gains in type safety with a minimal performance cost.

## Usage

Augmented Types requires that all type-enforced functions and methods be annotated with valid PHPDoc that specifies a type for the return value and every argument, like so:
```php
/**
* @param int $a
* @return float[]
*/
function foo ($a) {
	echo "You passed in the integer $a";
	return [$a * 1.0, $a * 2.718];
}
```
At runtime, if any type differs from the expected type on a type-enforced function, Augmented Types will throw a fatal error. More information on [the type system](https://github.com/box/augmented_types/wiki/The-Type-System) and [type annotation requirements](https://github.com/box/augmented_types/wiki/Type-Annotation-Requirements) can be found on the wiki.

Augmented Types' master INI setting is `augmented_types.enforce_by_default`, which is considered to be 0 if not set.  That means, by default, Augmented Types will _only_ enforce type annotations on a given function if the file or directory the function is contained in is added to a whitelist. Augmented Types allows you to both whitelist and blacklist files and directories for enforcement. 

Alternatively, if you'd like to enable Augmented Types enforcement on all files, add `augmented_types.enforce_by_default=1` to your php.ini.

More information about whitelisting and blacklisting can be found [on the wiki](https://github.com/box/augmented_types/wiki/Whitelisting-and-Blacklisting).

## Installation

Compilation of Augmented Types is fairly straight-forward on any Unix-based system. Instructions can be found on the [installation wiki page](https://github.com/box/augmented_types/wiki/Installation) on how to compile, install, and properly configure Augmented Types. Unfortunately, at this time we do not support compilation on Windows systems, although we gladly welcome contributions to help support all environments.

## FAQ

The [FAQ](https://github.com/box/augmented_types/wiki/FAQ) has answers to many common questions, including how we use Augmented Types at Box and its performance characteristics. Information on contributing can be found [here](/CONTRIBUTING.md).

## Copyright and License

Copyright 2014 Box, Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
