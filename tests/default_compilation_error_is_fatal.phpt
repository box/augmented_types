--TEST--
Test to ensure that the default compilation error is throwing a fatal
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php
require_once "tests/helper/invalid1.php"; // this should throw warnings instead
?>
--EXPECTREGEX--
Fatal error: Function annotation compilation failed for function invalid1 in file .*invalid1.php at line (\d)*, error message: You MUST specify a type for all parameters
.*
