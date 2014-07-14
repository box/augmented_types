--TEST--
Test to ensure that inheritance in classes across different files works
--FILE--
<?php
echo augmented_types_whitelist(["tests/helper/superclass.php"])."\n";
echo augmented_types_blacklist(["tests/helper/subclass.php"])."\n";
require_once "tests/helper/subclass.php";

$a = new FooSub();
var_dump( $a->childMethod("foo") );
$a->parentMethod("foo");
?>
--EXPECTREGEX--
1
1
int\(42\)

Fatal error.*
.*
