--TEST--
Test that renamed trait methods retain phpdoc
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

trait Fooable {

	/**
	* @param int
	* @return int
	*/
	public function get($f) {
		return $f;
	}
}

class Foo {
	use Fooable {
		get as ret;
	}

}

$foo = new Foo();

echo $foo->ret(1) . "\n";

$foo->ret("hey");

?>
--EXPECTREGEX--
1

Fatal error: Wrong type encountered for argument 1 of function get, was expecting a integer but got a \(string\) 'hey'
.*
