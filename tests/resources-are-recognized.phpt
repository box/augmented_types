--TEST--
Resources are recognized and enforced
--INI--
augmented_types.enforce_by_default = 1
--FILE--
<?php

/**
* @return resource
*/
function getStdin() {
	return fopen("php://stdin", "r+");
}

$theResource = getStdin();
echo "hi!"
?>
--EXPECT--
hi!