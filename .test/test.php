<?php

$schemas = ['clockwork', 'clockwork2'];
$query = "select * from `invoices` where `id` IN (?, ?, ?, ?, ?, ?)";
$bindings = array(1, 10, 14, 15, 16, 17);

print_r(runSelectCpp($schemas, $query, $bindings));




