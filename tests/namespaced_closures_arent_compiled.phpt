--TEST--
Namespaced closures arent compiled
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

namespace Organization\Fun;

class Foo
{
	/**
	 * @param int
	 * @return \Organization\Fun\Foo
	 */
	public function get($i)
	{
		echo "got this\n";
		$a = function($b) {
			echo $b + 42;
		};
		$a($i);
		return $this;
	}
}

$foo = new Foo();
$thing = $foo->get(42);

?>
--EXPECT--
got this
84
