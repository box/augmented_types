--TEST--
Test to ensure that blacklisting works
--FILE--
<?php
echo augmented_types_blacklist(["tests/helper"])."\n";
echo augmented_types_whitelist(["tests/helper/"])."\n";
require_once "tests/helper/invalid2.php"; // shouldn't be compiled

echo augmented_types_whitelist(["tests/helper/invalid1.php"])."\n";
require_once "tests/helper/invalid1.php"; // should be compiled!
?>
--EXPECTREGEX--
1
1
1

Fatal error.*
.*
