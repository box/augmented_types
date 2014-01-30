<?php

/**
 * This function whitelists an array of directories and files. The paths
 * contained in $paths can be either relative or absolute, as long as they
 * point to valid directories or files. This function returns an integer
 * indicating how many valid paths were added to the whitelist. For example,
 * this would be a valid $paths array: ['.', getcwd()]
 *
 * @param array $paths - The array of paths to add to the whitelist
 * @return int - the number of valid paths added to the whitelist
 */
function augmented_types_whitelist(array $paths) {}

/**
 * This function blacklists an array of directories and files. The paths
 * contained in $paths can be either relative or absolute, as long as they
 * point to valid directories or files. This function returns an integer
 * indicating how many valid paths were added to the blacklist. For example,
 * this would be a valid $paths array: ['.', getcwd()]
 *
 * @param array $paths - The array of paths to add to the blacklist
 * @return int - the number of valid paths added to the blacklist
 */
function augmented_types_blacklist(array $paths) {}
