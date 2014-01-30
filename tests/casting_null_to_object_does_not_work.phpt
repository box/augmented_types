--TEST--
Test casting null to object type does not work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
class Foo {
	
}

/**
* @param Foo $foo
* @return void
*/
function take_foo($foo) {
	
}

$i = null;
// take_foo((Foo) $i); you can't cast userland objects!
take_foo(null);

?>
--EXPECTREGEX--
Fatal error: Wrong type encountered for argument 1 of function take_foo, was expecting a Foo but got a null
.*
