
<?php

    function checkFile($fileExtension, $fileTmpPath) {

        //check File size
        $fileSize = $_FILES['uploaded-file']['size'];
        $maxFileSize = 2 * 1024 * 1024; // 2 MB
        if ($fileSize > $maxFileSize) 
            return "Error: The file is too large. Maximum allowed size is 2 MB.";

        //check File Extension
        if ($fileExtension !== "jpg" && $fileExtension !== "jpeg" && $fileExtension !== "png") {
                      return "The file format you tried to upload is not supported on our server. Please do only upload .jpg, .jpeg or .png files.";
        }

        //check File Type
        $allowedFileType = ['image/jpeg', 'image/jpg', 'image/png'];
        $fileType = $_FILES['uploaded-file']['type'];
        if (!in_array($fileType, $allowedFileType)) {
                return "The file format you tried to upload is not supported on our server. Please do only upload .jpg, .jpeg or .png files.";
        }

        // Check MIME Type using finfo_file
        $fileTmpPath = $_FILES['uploaded-file']['tmp_name'];
        $finfo = finfo_open(FILEINFO_MIME_TYPE); // Return mime type
        $mimeType = finfo_file($finfo, $fileTmpPath);
        finfo_close($finfo);
        $allowedMimeTypes = ['image/jpeg', 'image/png'];
        if (!in_array($mimeType, $allowedMimeTypes)) {
          return "The file format you tried to upload is not supported on our server. Please upload only .jpg, .jpeg, or .png files.";
        }

        // Check if the file content is a valid image
        $imageData = getimagesize($fileTmpPath);
        if ($imageData === false) {
            return "The uploaded file is not a valid image.";
        }

        else
            return '';
    }

// Check if the file was uploaded without errors
    if (isset($_FILES['uploaded-file']) && $_FILES['uploaded-file']['error'] === UPLOAD_ERR_OK) {
        $fileTmpPath = $_FILES['uploaded-file']['tmp_name'];
        $fileName = $_FILES['uploaded-file']['name'];
        $fileNameCmps = explode(".", $fileName);
        $fileExtension = strtolower(end($fileNameCmps));
        $message = checkFile($fileExtension, $fileTmpPath);

        if ($message == '') {
            $newFileName = preg_replace("/[^a-zA-Z0-9_-]/", "-", ($_POST['name'] . "_" . $_POST['title']))  . '.' . $fileExtension;
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

	?>