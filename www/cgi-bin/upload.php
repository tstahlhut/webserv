#!/usr/bin/php
<?php
// Set content type to HTML - include if we change the way of how we build the response
//header('Content-Type: text/html');

// Check if the form was submitted - right now it is not necessary because we check it in the server
//if ($_SERVER['REQUEST_METHOD'] == 'POST') {
//}
//sleep(10);
// Check if the request size exceeds a limit
if (isset($_SERVER['CONTENT_LENGTH'])) {
    $contentLength = (int) $_SERVER['CONTENT_LENGTH']; // Size of the POST request in bytes
    $maxSize = 8388608; // 8MB (8388608 bytes)

    if ($contentLength > $maxSize)
        $message="The image you tried to upload exceeds the maximum file size.";
    else
        $message='';
}

// Include head & header
include 'head.php';
include 'header.php';

// Main content
?>
<main>
    <div class='grid'>
        
        
        <?php
	// Evaluate passed name and title
    if ($message === '')
      include 'evaluate_form_data.php';
    
    if ($message === '') {
        // Evaluate uploaded file (only if there are no errors with the other form data)
        include 'evaluate_file_upload.php';
    }

    // build response message
      if ($message) {
           echo "<h1 id='grid-header'>Error</h1>
          <p class='form-text' id='upload-intro'>$message</p>
           <a  id='form-link-btn' href='../upload.html'><button class='submit-btn'>Back to form</button></a>"; 
        }
        else {
       	echo "<h1 id='grid-header'>Your artwork was added!</h1>
			<p class='form-text' id='upload-intro'>Thank you, $name, for your amazing artwork!<br><br>
			'$title' is now part of our artwork community and<br> can be viewed, feedbacked, and liked by other artists of the community.</p>
			<a  id='form-link-btn' href='/gallery.py'><button class='submit-btn'>Visit Gallery</button></a>";
       }
?>    
 	</div>
</main>

<?php
// Include the footer
include 'footer.php';
?>
