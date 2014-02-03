--TEST--
Test that imported namespaces work
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

include "tests/helper/namespace1.php";
include "tests/helper/namespace2.php";

$class2 = new Vendor\Package2\Class2();
$class2->test(new Vendor\Package1\Class1());

?>
--EXPECTREGEX--
success!
