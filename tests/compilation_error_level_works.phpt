--TEST--
Test to ensure that changing the compilation error reporting level works
--INI--
augmented_types.enforce_by_default = 1
augmented_types.compilation_error_level = E_WARNING
--FILE--
<?php
require_once "tests/helper/invalid1.php"; // this should throw warnings instead
?>
--EXPECTREGEX--
Warning: Function annotation compilation failed for function invalid1 in file .*invalid1.php at line (\d)*, error message: You MUST specify a type for variable \$x
.*
