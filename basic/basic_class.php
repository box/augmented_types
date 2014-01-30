<?php

class Test
{
	private $x;

	public function foo()
	{
		echo "Sup man, its foo!\n";
	}

	/**
	 * @param int $y
	 */
	public function inc_int($y)
	{
		$this->x += 1;
		return $this->x + $y;
	}
}
