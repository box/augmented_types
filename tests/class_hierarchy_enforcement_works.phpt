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

class Baz extends Bar
{
	/**
	 * @return void
	 */
	public function bazbaz()
	{
		echo "baz\n";
	}
}

$o = new Baz();
var_dump( $o->inc_int(2) );
$o->barbar();
$o->bazbaz();
$o->inc_int("foo");

?>
--EXPECTREGEX--
int\(44\)
barbarbar
baz

Fatal error:.*
.*
