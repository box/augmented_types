<?php

$subtractor_string = '
/**
 * This subtracts 2 numbers, real cool
 * @param int $x The coolest thing ever
 * @param int $y An awesome integer
 * @return int
 */
function subtractor($x, $y)
{
	return $x - $y;
}
';

eval($subtractor_string);
