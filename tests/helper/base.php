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

	public function __construct()
	{
		echo "hello\n";
	}
}
