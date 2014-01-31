--TEST--
Ensure that a function that specifies a return type but actually returns nothing will
_always_ be caught.
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @param string
 * @return null
 */
function do_something($a) {
	echo $a;
}

do_something("hello!");

?>
--EXPECTREGEX--
.*was expecting a string\|null but got a void.*
