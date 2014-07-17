--TEST--
Stress test the AT compiler with lots of files and classes
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

$f = create_function('$i', 'return $i + 42;');
var_dump( $f(42) );

$a = [1,2,3];
array_walk($a, create_function('&$v, $k', '$v = $v + 42;'));
var_dump( $a[0] );
?>
--EXPECT--
int(84)
int(43)
