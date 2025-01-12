#!/usr/bin/env python3
#!/usr/bin/env python3

import cgi
import os

# get root directory from env, "www" is default
document_root = os.getenv("ORIGINAL_ROOT")
cgi_root = os.getenv("DOCUMENT_ROOT")

# Function to read html content from other files
def include_html(filename):
	path = os.path.join(os.path.abspath(document_root), filename)
	with open(path, 'r') as file:
		return file.read()

def include_html_from_cgi(filename):
	path = os.path.join(os.path.abspath(cgi_root), filename)
	with open(path, 'r') as file:
		return file.read()

head_content = include_html('head.html')
header_content = include_html_from_cgi('header.php')
footer_content = include_html_from_cgi('footer.php')

# Build http content

print(f"""
 	{head_content}
	{header_content}

	<main>	
		<section class="banner">
			<h1>TEST: Python CGI</h1>
			<h2>Yes yes yes!</h2>
			<h2>Our CGI does work with GET and Python!</h2>
		</section>	
		
	</main>
	{footer_content}
""")