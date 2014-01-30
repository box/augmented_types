<?php

/**
 * This adds 2 numbers, fuckin' sweet
 * @param int $x The coolest thing ever
 * @param int $y An awesome integer
 * @return int
 */
function addr($x, $y)
{
	return $x + $y;
}

/**
 * @param int $x
 * @return int
 */
function noop($x)
{
	echo $x." is a noop!\n";
}

/**
 * This is cool
 * So cool it deserves a second line
 * @param float $x This is a super cool float
 * @param int $y
 * @param bool $z
 * @return float
 */
function mixed_type_adder($x, $y, $z)
{
	// thanks for letting us do this, php
	return $x + $y + $z;
}

/**
 * @param int $x my awesome integer
 * @return int
 */
function wrongRet($a)
{
	return $a + 1.0;
}

/**
 * @param int $x
 * @param bool $y
 * @return int
 */
function ifaddr($x, $y)
{
	if (!($y === true)) {
		return $x + 42;
	}
	return 24;
}

/**
 * @param int $x
 * @param float $y
 * @param int $z
 * @return float
 */
function opt_addr($x, $y = 0.1, $z = 1)
{
	return $x + $y + $z;
}
