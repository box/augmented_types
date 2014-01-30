--TEST--
Void works when invoked via reflection
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @return void
*/
function foo()
{
	echo "hi!";
}

$fn = new ReflectionFunction("foo");
$fn->invoke();

?>
--EXPECT--
hi!