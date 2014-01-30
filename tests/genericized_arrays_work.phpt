--TEST--
Test that genericized arrays work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @param mixed $thing
* @return int[]|int[][]
*/
function get_array($thing) {
	print_r($thing);
	return $thing;
}

/**
* @param mixed $arr_value
* @return void
*/
function print_unexval_exception($arr_value) {
	try {
		$a = get_array($arr_value);
	} catch (UnexpectedValueException $e) {
		echo $e->getMessage() . "\n";
	}
}


$a = get_array([1,2,3]);
$b = get_array([[1,2], [3,4]]);

print_unexval_exception([1,null]);
//print_unexval_exception([[[]]]);
//print_unexval_exception([[1,2], 1]);
//print_unexval_exception([1, [1,2]]);

?>
--EXPECTREGEX--
Array
\(
    \[0\] => 1
    \[1\] => 2
    \[2\] => 3
\)
Array
\(
    \[0\] => Array
        \(
            \[0\] => 1
            \[1\] => 2
        \)

    \[1\] => Array
        \(
            \[0\] => 3
            \[1\] => 4
        \)

\)
Array
\(
    \[0\] => 1
    \[1\] => 
\)

Fatal error: Wrong type encountered for return value of function get_array, was expecting a \(integer\)\[\]|\(\(integer\)\[\]\)\[\] but got a array
.*
