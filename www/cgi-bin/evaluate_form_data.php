<?php
 // Get input data (from env), decode and sanitize them (delete html or php code)
//$name = strip_tags(urldecode(getenv('name')));
//$title = strip_tags(urldecode(getenv('title')));

// Get input data (from env), decode and sanitize with htmlspecialchars() which also prevents XSS attacks by converting special characters
//$name = htmlspecialchars(urldecode(getenv('name')), ENT_QUOTES, 'UTF-8');
//$title = htmlspecialchars(urldecode(getenv('title')), ENT_QUOTES, 'UTF-8');
$name = htmlspecialchars($_POST['name']); // Use htmlspecialchars to prevent XSS
$title = htmlspecialchars($_POST['title']);

//print_r($_ENV);
//Question: which one would we like?
//ex: user input <h1>Picasso</h1>
//with strip_tags, output: Picasso
//with htmlspecialchars: <h1>Picasso</h1>

if (empty($name) | empty($title)) {
	$message = 'Please fill in both the name of the artist and the title of the artwork.';
}
else if (strlen($name) > 30 | strlen($title) > 40) {
   $message = 'The name of the artist should not be longer than 30 characters and the title of the artwork not longer than 40 characters. Please try again with shorter names or reach out to us.';
}
else {
	$message = '';
}
?>