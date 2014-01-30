--TEST--
Ensure that not passing a value will be warned against when specifying null
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @param string|null
 * @return void
 */
function do_something($a = "foo") {
	echo $a;
}

do_something();

?>
--EXPECTREGEX--
.*was expecting a string\|null but no argument was passed.*
