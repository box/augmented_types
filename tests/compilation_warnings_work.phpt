--TEST--
Test numeric types allow strings that have numeric value
--INI--
augmented_types.enforce_by_default = 1
augmented_types.compilation_error_level = E_WARNING
--FILE--
<?php

/**
* @param numeric $param
*/
function this_accepts_numerics($number) {
	return $number * 3;
}

/**
* @param $param
* @return void
*/
function this_echo($x) {
	echo $x;
}

/**
 * @param int $x
 * @return int
 */
function foo($x) {
	return $x + 42;
}

echo foo(0);
?>
--EXPECTREGEX--
Warning: Function annotation compilation failed for function this\_accepts\_numerics in file .*, error message: Return types must always be specified.
.*

Warning: Function annotation compilation failed for function this\_echo in file .*, error message: You MUST specify a type for all parameters
.*
42
