--TEST--
Namespaced classes expand local typehints to their full namespace before storing them
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

namespace Organization\Fun;

require "tests/helper/baz_class.php";

class Foo
{
	/**
	 * @return Foo
	 */
	public function get()
	{
		echo "got this\n";
		return $this;
	}
}

/**
 * @param Foo
 * @return void
 */
function bar($foo)
{
	echo "bar bar BAR\n";
}

$foo = new Foo();
$thing = $foo->get();
bar($foo);

/**
 * @return \Baz
 */
function bazzer() {
	$baz = new \Baz();
	$baz->bazzer();
	return $baz;
}
$b = bazzer();

?>
--EXPECT--
got this
bar bar BAR
bazzing
