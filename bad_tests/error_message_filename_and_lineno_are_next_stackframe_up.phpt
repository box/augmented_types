--TEST--
Ensure that argument error messages always list the place that calls the function as the offender
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
require "tests/helper/valid.php";

/**
 * The function 'valid' is a function that takes an integer
 * and adds 42 to it. It should barf on a string being passed
 * @param int|string $a
 * @return int
 */
function loose_adder($a) {
	return valid($a);
}

echo loose_adder(3)."\n";
echo loose_adder("3")."\n"; // this should barf on line 16 in the phpt file, which
							// is line 11 in the generated php file

//// BIG NOTE: this test is currently BROKEN, due to zend_error blowing it. The line number is taken from the
//// caller but the filename is taken from the callee (see functions 'zend_get_executed_lineno' and
//// 'zend_get_executed_filename' called from 'zend_error' defined in zend.c). This is quite unfortunate...
//// This is broken in php 5.4.19, not verified broken anywhere else

?>
--EXPECTREGEX--
45

Fatal error: Wrong type encountered for argument 1 of function valid, was expecting a integer|string but got a \(string\) '3'
in (.*)tests/error_message_line_number_should_be_next_stackframe_up.php on line 11
