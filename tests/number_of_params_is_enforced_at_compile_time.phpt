--TEST--
Test that mismatches in the number of params in phpdoc to the number of params in a function
are caught at compile time
--INI--
augmented_types.enforce_by_default = 1
augmented_types.compilation_error_level = E_WARNING
--FILE--
<?php
/**
 * @param int $foo
 */
function f() { echo "f\n"; }

/**
 * @param int $foo
 * @return void
 */
function foo() { echo "foo\n"; }

/**
 * @return void
 */
function bar($b) { echo $b."foo\n"; }

/**
 * @param string $foo
 * @return void
 */
function baz($b, $c = "") { echo $b.$c."foo\n"; }

/**
 * @param string $foo
 * @return void
 */
function boo($a = "", $b = "", $c = "") { echo $b.$c."foo\n"; }

?>
--EXPECTREGEX--
Warning: Function annotation compilation failed for function boo in file.*, error message: The number of PHPDoc parameters given does not match the actual number of function parameters.
.*

Warning: Function annotation compilation failed for function baz in file.*, error message: The number of PHPDoc parameters given does not match the actual number of function parameters.
.*

Warning: Function annotation compilation failed for function bar in file.*, error message: The number of PHPDoc parameters given does not match the actual number of function parameters.
.*

Warning: Function annotation compilation failed for function foo in file.*, error message: The number of PHPDoc parameters given does not match the actual number of function parameters.
.*

Warning: Function annotation compilation failed for function f in file.*, error message: Return types must always be specified.
.*
