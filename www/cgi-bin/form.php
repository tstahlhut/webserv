#!/usr/bin/php
<?php
// Set content type to HTML
//header('Content-Type: text/html');

// Check if the form was submitted
//if ($_SERVER['REQUEST_METHOD'] == 'POST') {
//}

// Include head & header
include 'head.php';
include 'header.php';


// Get input data (from env), decode and sanitize them (delete html or php code)
//$name = strip_tags(urldecode(getenv('name')));
//$title = strip_tags(urldecode(getenv('title')));

// Get input data (from env), decode and sanitize with htmlspecialchars() which also prevents XSS attacks by converting special characters
//$name = htmlspecialchars(urldecode(getenv('name')), ENT_QUOTES, 'UTF-8');
//$title = htmlspecialchars(urldecode(getenv('title')), ENT_QUOTES, 'UTF-8');
$name = htmlspecialchars($_POST['name']); // Use htmlspecialchars to prevent XSS
$title = htmlspecialchars($_POST['title']);
$test = isset($_POST['name']) ? htmlspecialchars($_POST['name']): 'name not set';

//print_r($_ENV);
//Question: which one would we like?
//ex: user input <h1>Picasso</h1>
//with strip_tags, output: Picasso
//with htmlspecialchars: <h1>Picasso</h1>

// Main content
?>
<main>
	<div class='grid'>
	

 <?php
	
    if (empty($name) | empty($title)) {
        $message = 'Please fill in both the name of the artist and the title of the artwork.';
    }
	else if (strlen($name) > 30 && strlen($title) > 40) {
       $message = 'The name of the artist should not be longer than 30 characters and the title of the artwork not longer than 40 characters. Please try again with shorter names or reach out to us.';
    }
     // Check if the file was uploaded without errors
    else if (isset($_FILES['uploaded-file']) && $_FILES['uploaded-file']['error'] === UPLOAD_ERR_OK) {
        $fileTmpPath = './tmp';//$_FILES['uploaded-file']['tmp_name'];
        $fileName = $_FILES['uploaded-file']['name'];
        //$fileSize = $_FILES['uploaded-file']['size'];
      //  $fileType = $_FILES['uploaded-file']['type'];
        $fileNameCmps = explode(".", $fileName);
        $fileExtension = strtolower(end($fileNameCmps));

        // Sanitize the file name
        $newFileName = md5(time() . $fileName) . '.' . $fileExtension;

        // Define the upload directory path
        $uploadFileDir = './uploaded_files/';
        $dest_path = $uploadFileDir . $newFileName;

        // Ensure the upload directory exists
        if (!is_dir($uploadFileDir)) {
            mkdir($uploadFileDir, 0755, true);
        }

        // Move the file from the temporary directory to the destination
        if (!move_uploaded_file($fileTmpPath, $dest_path)) {
            $message = 'There was an error moving the uploaded file.';
        }
    }
    else {
        $errorcode = $_FILES['uploaded-file']['error'];
        switch ($errorcode) {
            case UPLOAD_ERR_INI_SIZE:
                $message = 'The uploaded file exceeds the maximum file size of 2M. Please compress your image file.';
                break;
           // case UPLOAD_ERR_FORM_SIZE:
            case UPLOAD_ERR_PARTIAL:
                $message = 'Your uploaded file was only partially uploaded.';
                break;
            case UPLOAD_ERR_NO_FILE:
                $message = 'You did not upload any file.';
                break;
            default:
                $message = 'There occurred an error during file upload.';
        }
    }
    $user= get_current_user();
    if ($message) {
        echo "<h1 id='grid-header'>Error</h1>
        <p class='form-text' id='upload-intro'>$message;</p>
        <a  id='form-link-btn' href='../upload.html'><button class='submit-btn'>Back to form</button></a>"; 
    }
    else {
    
	echo "<h1 id='grid-header'>Your artwork was added!</h1>
					<p class='form-text' id='upload-intro'>Thank you, $name, for your amazing artwork!<br><br>
					'$title' is now part of our artwork community and<br> can be viewed, feedbacked, and liked by other artists of the community.
					These are the variables of the uploaded file: 
					$fileTmpPath, $fileName.				</p>
					<a  id='form-link-btn' href='../get_started.html'><button class='submit-btn'>Visit Gallery</button></a>";
    }
?>    
 	</div>
</main>

<?php
// Include the footer
include 'footer.php';
?>
