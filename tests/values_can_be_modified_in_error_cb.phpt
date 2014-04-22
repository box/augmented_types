--TEST--
Ensure that the augmented types error callback can safely modify values with the same semantics as normal php functions
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
 * @param mixed|null $offending_value
 * @param bool $is_void
 * @param string $expected_type
 * @param string $function_name
 * @param string $file_path
 * @param int $line_number
 * @param int $arg_num
 * @return void
 */
function error_cb($offending_value, $is_void, $expected_type, $function_name, $file_path, $line_number, $arg_num)
{
	if ($arg_num === -1) {
		echo "error in return type! value = $offending_value\n";
		$offending_value += 1;
	}
	if (is_array($offending_value)) {
		echo "got an array!\n";
		$offending_value[] = "MODIFIED";
	}
}

augmented_types_register_type_error_callback("error_cb");

/**
* @param int input
* @return uint output
*/
function negate($i) {
	return $i;
}
$ret = negate(-1);
var_dump($ret);

/**
* @param int
* @return array
*/
function impossible($i) {
	return $i;
}

$a = impossible(['hello']);
var_dump($a);

?>
--EXPECT--
error in return type! value = -1
int(-1)
got an array!
array(1) {
  [0]=>
  string(5) "hello"
}
