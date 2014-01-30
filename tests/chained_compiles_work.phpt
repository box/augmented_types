--TEST--
Test to ensure that zend's weird optimization when compiling an empty class doesn't screw us over
--FILE--
<?php
echo augmented_types_blacklist(["tests/helper/"])."\n";
require_once "tests/helper/invalid_chained_class.php";
require_once "tests/helper/invalid_chained_func.php";
echo augmented_types_whitelist(["tests/helper/chained_class.php"])."\n";
echo augmented_types_whitelist(["tests/helper/chained_func.php"])."\n";
require_once "tests/helper/chained_class.php";
require_once "tests/helper/chained_func.php";
?>
--EXPECT--
1
1
1
