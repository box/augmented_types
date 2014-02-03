--TEST--
Ensure that parentheses correctly associate types
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param (integer|string)
* @return void
*/
function take_string_or_int($thing) {
	if (is_string($thing)) {
		echo "thing is string\n";
	}
	else {
		echo "thing is int\n";
	}
}

take_string_or_int("foo");
take_string_or_int(1);

/**
* @param *(integer|string)
* @return void
*/
function take_string_or_int_var() {
	$args = func_get_args();
	if (is_string($args[0])) {
		echo "thing is string\n";
	}
	else {
		echo "thing is int\n";
	}
}

take_string_or_int_var(0, 1, "foobar");

/**
* @param (int|string)|void $interesting
* @return void
*/
function take_mixed($a = 0) {
	if (is_string($a)) {
		echo "thing is string\n";
	}
	else {
		echo "thing is int\n";
	}
}

take_mixed();

/**
* @param (int|string)[] $thing
* @return (int|string)[]
*/
function take_string_or_int_arr($thing) {
	if (is_string($thing[0])) {
		echo "arr starts with string\n";
	}
	else {
		echo "arr starts with int\n";
	}
	return $thing;
}
$a = take_string_or_int_arr(["foo"]);
$b = take_string_or_int_arr([1, 'bar']);

/**
* @param (int|(string[]))[]
* @return (int|(string[]))[]
*/
function take_complex_type($thing) {
	echo "a complex type LOL\n";
	return $thing;
}

$a = take_complex_type([2]);
$b = take_complex_type([['lolz']]);
$c = take_complex_type([['lolz', 'as if'], 1234, 9876]);
$d = take_complex_type(['not the right type']);

/**
* @param int|string[]
* @return int|string[]
*/
function foo($bar) {
	return $bar;
}

?>
--EXPECTREGEX--
thing is string
thing is int
thing is int
thing is int
arr starts with string
arr starts with int
a complex type LOL
a complex type LOL
a complex type LOL
Fatal error: Wrong type encountered for return value of function take_complex_type, was expecting a \(integer\[\]|\(string\)\[\]\)\[\] but got a array
.*
