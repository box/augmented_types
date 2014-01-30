<?php

$iters = 5000000;

/**
 * Adds a bunch of stuff
 * @param int x
 * @param float y
 * @param float x
 * @return float
 */
function computer($x, $y, $z)
{
	$a = $x * $y * $z;
	$b = 0;
	while ($b < 10) {
		$a = $a / 2.0 * $y / $z;
		$b = $b + 1;
	}
	return $a;
}

for($i = 0; $i < $iters; $i ++){
	$t = computer(2, 3.14, 3.14159);
}

echo $t."\n\n";
