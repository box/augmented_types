--TEST--
Test to ensure that zend's weird optimization when compiling subclasses doesn't corrupt memory
--FILE--
<?php
echo augmented_types_blacklist(["tests/helper/"])."\n";
require_once "tests/helper/superclass.php";
echo augmented_types_whitelist(["tests/helper/superclass.php"])."\n";
echo augmented_types_whitelist(["tests/helper/subclass.php"])."\n";
require_once "tests/helper/subclass.php";
?>
--EXPECTREGEX--
1
1
1

Fatal error: Compilation failed for function parentMethod in file .*superclass.php due to a duplicate reference, and its type information won't be enforced. This likely happened because the file was added to the whitelist after it was required during execution.
.*
