--TEST--
Test ensures that not specifying an arg type for an argument doesn't work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @param $x
 * @return void
 */
function bar($x) {
	return 42;
}

?>
--EXPECTREGEX--
.*Function annotation compilation failed for function bar in file .*ensure_not_specifying_argtype_fails.php at line (\d)*, error message: You MUST specify a type for all parameters.*
