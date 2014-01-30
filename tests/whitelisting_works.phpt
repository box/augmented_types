--TEST--
Test to ensure that whitelisting works
--FILE--
<?php
require_once "tests/helper/invalid1.php"; // this isn't whitelisted or blacklisted yet, so it shouldn't be compiled
echo augmented_types_blacklist(["/"])."\n";
echo augmented_types_whitelist(["tests/helper/invalid2.php"])."\n";
require_once "tests/helper/invalid2.php"; // whitelist is longer
?>
--EXPECTREGEX--
1
1

Fatal error.*
.*
