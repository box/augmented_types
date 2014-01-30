--TEST--
Test to ensure that compilation works with class hierarchy inheritance
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
abstract class Foo
{
	private $x = 0;

	/**
	 * @param int $y
	 * @return int
	 */
	public function inc_int($y)
	{
		return $this->x + 42 + $y;
	}
}

class Bar extends Foo
{
	/**
	 * @return void
	 */
	public function barbar()
	{
		echo "barbarbar\n";
	}
}

$o = new Bar();
echo $o->inc_int(2);
?>
--EXPECT--
44
