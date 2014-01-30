--TEST--
Namespaced classes are correctly tested
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

namespace Organization\Fun;

class Foo
{
	/**
	 * @return \Organization\Fun\Foo
	 */
	public function get()
	{
		echo "got this\n";
		return $this;
	}

	/**
	 * @return Foo
	 */
	public function getAgain()
	{
		echo "got this again";
		return $this;
	}
}

$foo = new Foo();
$thing = $foo->get()->getAgain();

?>
--EXPECT--
got this
got this again
