--TEST--
Test ensures that not specifying a return value for a function doesn't work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @param Foo $x
 */
function bar($x) {
	return 42;
}

?>
--EXPECTREGEX--
.*Function annotation compilation failed for function bar in file .*ensure_not_specifying_returns_fails.php at line (\d)*, error message: Return types must always be specified.*
