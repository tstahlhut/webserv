#!/usr/bin/python3

import os
import cgi

# Parse form data
form = cgi.FieldStorage()
method = form.getvalue("_method")
filename = form.getvalue("filename")
file_path = form.getvalue("filepath")

# get root directory from env, "www" is default
document_root = os.getenv("ORIGINAL_ROOT", "www")
cgi_root = os.getenv("DOCUMENT_ROOT", "www/cgi-bin")

# Function to read html content from other files
def include_html(filename):
	path = os.path.join(os.path.abspath(document_root), filename)
	with open(path, 'r') as file:
		return file.read()

def include_html_from_cgi(filename):
	path = os.path.join(os.path.abspath(cgi_root), filename)
	with open(path, 'r') as file:
		return file.read()

# Function for deleting files
def delete_image(filename, file_path):
	try:
		os.remove(file_path)
		return f"{filename}' was deleted sucessfully."
	except FileNotFoundError:
		return f"We couln't find '{filename}' in our gallery."
	except Exception as e:
		return f"Error deleting image '{filename}'."

message = ""
if method == "DELETE" and filename and file_path:
	message = delete_image(filename, file_path)


# Get uploaded images
upload_directory = os.path.join(os.path.abspath(cgi_root),"uploaded_files")

image_files = []

if os.path.exists(upload_directory) :
	image_files = [f for f in os.listdir(upload_directory)]


head_content = include_html('head.html')
header_content = include_html_from_cgi('header.php')
footer_content = include_html_from_cgi('footer.php')

	
# Print head, header and banner
print(f"""

{head_content}
{header_content}

<main>	
		<section class="banner">
			<h1>Gallery</h1>	

""")

# Check if there are any images
if not image_files:
	print("<h2>No images uploaded yet.</h2>")

if message:
	print(f"<h2 style='color: red;'>{message}</h2>")

print("</section>")

if image_files:
    print("<section class='image-gallery'>")

    for image_file in image_files:
        image_path = os.path.join(upload_directory, image_file)
        print(f"""<div class='image-item'>
     		<img src='cgi-bin/uploaded_files/{image_file}' alt='{image_file}' />
     		<p>{image_file}</p>
			<form action="/gallery.py" method="POST" onsubmit="return confirm('Are you sure you want to delete this image?');">
				<input type="hidden" name="_method" value="DELETE">
				<input type="hidden" name="filepath" value="{image_path}">
				<input type="hidden" name="filename" value="{image_file}">
				<button type="submit">Delete</button>
       		</form>
			</div>""")

    print("</section>")

print("</main>")
print(f"""{footer_content}""")
