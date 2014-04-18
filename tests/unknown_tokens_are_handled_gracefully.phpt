--TEST--
Unknown lexer tokens are handled gracefully
--INI--
augmented_types.enforce_by_default = 1
augmented_types.compilation_error_level = E_WARNING
--FILE--
<?php

/**
* @param &int $param
* @return int
*/
function foo($number) {
	return $number * 3;
}

/**
* @param foo
* @param $$#*@
* @return void
*/
function this_echo($x, $y) {
	echo $x;
}

/**
* @param foo asl;dkdskl
* @return &$# hello
*/
function baz($i) {
	echo $i;
}

/**
 * @param int $x
 * @return int
 */
function bar($x) {
	return $x + 42;
}

echo bar(0);
?>
--EXPECTREGEX--
Warning: Function annotation compilation failed for function foo in file .*, error message: Invalid parameter type - unknown character encountered
.*

Warning: Function annotation compilation failed for function this\_echo in file .*, error message: Invalid parameter type - unknown character encountered
.*

Warning: Function annotation compilation failed for function baz in file .*, error message: Invalid return type - unknown character encountered
.*
42
