#!/usr/bin/env python3
"""
Kotha IDE Server - Fixed Version
Properly connects to Kotha compiler
"""

import http.server
import socketserver
import subprocess
import os
import json
import tempfile

import cgi
import shutil

PORT = 8081

# Path to Kotha compiler executable
KOTHA_BIN = os.path.abspath(os.path.join(os.path.dirname(__file__), "../kotha/kotha"))

# Path to example Kotha files
KOTHA_EXAMPLES = os.path.abspath(os.path.join(os.path.dirname(__file__), "../kotha"))

# Change to server directory
os.chdir(os.path.dirname(os.path.abspath(__file__)))

class KothaHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        # Serve example files from examples directory
        if self.path.startswith("/examples/"):
            filename = self.path[10:]  # Remove "/examples/" prefix
            filepath = os.path.join(os.path.dirname(__file__), "examples", filename)
            
            if os.path.isfile(filepath):
                try:
                    with open(filepath, 'r') as f:
                        content = f.read()
                    
                    self.send_response(200)
                    self.send_header('Content-type', 'text/plain; charset=utf-8')
                    self.send_header('Access-Control-Allow-Origin', '*')
                    self.end_headers()
                    self.wfile.write(content.encode('utf-8'))
                    return
                except Exception as e:
                    self.send_error(500, f"Error reading file: {str(e)}")
                    return
            else:
                self.send_error(404, f"File not found: {filename}")
                return
        
        # Serve example Kotha files from kotha directory
        if self.path.startswith("/kotha/"):
            filename = self.path[7:]  # Remove "/kotha/" prefix
            filepath = os.path.join(KOTHA_EXAMPLES, filename)
            
            if os.path.isfile(filepath):
                try:
                    with open(filepath, 'r') as f:
                        content = f.read()
                    
                    self.send_response(200)
                    self.send_header('Content-type', 'text/plain; charset=utf-8')
                    self.send_header('Access-Control-Allow-Origin', '*')
                    self.end_headers()
                    self.wfile.write(content.encode('utf-8'))
                    return
                except Exception as e:
                    self.send_error(500, f"Error reading file: {str(e)}")
                    return
            else:
                self.send_error(404, f"File not found: {filename}")
                return
        
        if self.path == "/api/files":
            examples_dir = os.path.join(os.path.dirname(__file__), "examples")
            
            def get_files_recursive(directory, relative_path=""):
                items = []
                if not os.path.exists(directory):
                    return items
                    
                for name in sorted(os.listdir(directory)):
                    full_path = os.path.join(directory, name)
                    rel_path = os.path.join(relative_path, name)
                    
                    if os.path.isdir(full_path):
                        items.append({
                            "name": name,
                            "path": rel_path,
                            "type": "folder",
                            "children": get_files_recursive(full_path, rel_path)
                        })
                    elif name.endswith('.kotha'):
                        items.append({
                            "name": name,
                            "path": f"examples/{rel_path}",
                            "type": "file"
                        })
                return items

            files = get_files_recursive(examples_dir)
            
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps({"files": files}).encode('utf-8'))
            return
        
        # Default routing for other files
        if self.path == "/":
            self.path = "/public/index.html"
        elif self.path.startswith("/public/"):
            pass
        else:
            self.path = "/public" + self.path
        return http.server.SimpleHTTPRequestHandler.do_GET(self)

    def do_POST(self):
        # File Creation
        if self.path == "/api/files/create":
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            filename = data.get('path', '')
            
            # Sanitize and validate path (force inside examples/)
            if '..' in filename or filename.startswith('/'):
                self.send_error(400, "Invalid filename")
                return

            filepath = os.path.join(KOTHA_EXAMPLES, filename)
            
            try:
                # Create parent directories if they don't exist
                os.makedirs(os.path.dirname(filepath), exist_ok=True)
                
                if not os.path.exists(filepath):
                    with open(filepath, 'w') as f:
                        f.write('main function {\n    dekhaw("New File");\n}')
                
                self.send_response(200)
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps({"status": "created"}).encode('utf-8'))
            except Exception as e:
                self.send_error(500, str(e))
            return

        # Folder Creation
        if self.path == "/api/folders/create":
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            foldername = data.get('path', '')
            
            if '..' in foldername or foldername.startswith('/'):
                self.send_error(400, "Invalid folder name")
                return

            folderpath = os.path.join(KOTHA_EXAMPLES, foldername)
            
            try:
                os.makedirs(folderpath, exist_ok=True)
                self.send_response(200)
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps({"status": "created"}).encode('utf-8'))
            except Exception as e:
                self.send_error(500, str(e))
            return

        # File Upload
        if self.path == "/api/upload":
            ctype, pdict = cgi.parse_header(self.headers['content-type'])
            if ctype == 'multipart/form-data':
                pdict['boundary'] = bytes(pdict['boundary'], "utf-8")
                form = cgi.FieldStorage(
                    fp=self.rfile,
                    headers=self.headers,
                    environ={'REQUEST_METHOD': 'POST', 'CONTENT_TYPE': self.headers['Content-Type']}
                )
                
                try:
                    if 'file' in form:
                        fileitem = form['file']
                        filename = os.path.basename(fileitem.filename)
                        if not filename:
                             self.send_error(400, "No filename")
                             return
                             
                        # Save to examples directory
                        filepath = os.path.join(KOTHA_EXAMPLES, filename)
                        with open(filepath, 'wb') as f:
                            f.write(fileitem.file.read())
                            
                        self.send_response(200)
                        self.send_header('Access-Control-Allow-Origin', '*')
                        self.end_headers()
                        self.wfile.write(json.dumps({"status": "uploaded", "filename": filename}).encode('utf-8'))
                    else:
                        self.send_error(400, "No file field")
                except Exception as e:
                     self.send_error(500, str(e))
            return

        # File Save
        if self.path == "/api/files/save":
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            filename = data.get('path', '')
            content = data.get('content', '')
            
            # Sanitize and validate path (force inside examples/)
            if '..' in filename or filename.startswith('/'):
                self.send_error(400, "Invalid filename")
                return

            filepath = os.path.join(KOTHA_EXAMPLES, filename)
            
            try:
                # Ensure parent directory exists
                os.makedirs(os.path.dirname(filepath), exist_ok=True)
                
                with open(filepath, 'w') as f:
                    f.write(content)
                    
                self.send_response(200)
                self.send_header('Access-Control-Allow-Origin', '*')
                self.end_headers()
                self.wfile.write(json.dumps({"status": "saved"}).encode('utf-8'))
            except Exception as e:
                self.send_error(500, str(e))
            return

        if self.path == "/api/run":
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            code = data.get('code', '')
            inputs = data.get('inputs', [])  # Get pre-collected inputs

            # Create temp file for Kotha code
            with tempfile.NamedTemporaryFile(mode='w', suffix='.kotha', delete=False) as tmp:
                tmp.write(code)
                tmp_path = tmp.name

            # Create input file if inputs provided
            input_file = None
            if inputs:
                input_file = tmp_path.replace('.kotha', '.input')
                with open(input_file, 'w') as f:
                    f.write('\n'.join(str(inp) for inp in inputs) + '\n')

            try:
                # Step 1: Compile Kotha to C
                result = subprocess.run(
                    [KOTHA_BIN, tmp_path],  # Pass file path as argument
                    capture_output=True,
                    text=True,
                    timeout=10
                )
                
                if result.returncode != 0:
                    # Compilation failed
                    response = {
                        "stdout": "",
                        "stderr": result.stderr or "Compilation failed",
                        "exit_code": result.returncode
                    }
                else:
                    # Step 2: Save C code to file
                    c_file = tmp_path.replace('.kotha', '.c')
                    with open(c_file, 'w') as f:
                        f.write(result.stdout)
                    
                    # Step 3: Compile C to executable
                    exe_file = tmp_path.replace('.kotha', '.out')
                    
                    # Add Kotha standard libraries
                    kotha_dir = os.path.dirname(KOTHA_BIN)
                    file_io_o = os.path.join(kotha_dir, 'file_io.o')
                    string_lib_o = os.path.join(kotha_dir, 'string_lib.o')
                    
                    compile_cmd = ['gcc', c_file, '-o', exe_file]
                    
                    # Add standard libraries if they exist
                    if os.path.exists(file_io_o):
                        compile_cmd.insert(2, file_io_o)
                    if os.path.exists(string_lib_o):
                        compile_cmd.insert(2, string_lib_o)
                    
                    compile_result = subprocess.run(
                        compile_cmd,
                        capture_output=True,
                        text=True,
                        timeout=10
                    )
                    
                    if compile_result.returncode != 0:
                        response = {
                            "stdout": "",
                            "stderr": f"C compilation error:\n{compile_result.stderr}",
                            "exit_code": compile_result.returncode
                        }
                    else:
                        # Step 4: Run the executable
                        # If we have inputs, redirect stdin from input file
                        if input_file and os.path.exists(input_file):
                            with open(input_file, 'r') as stdin_file:
                                run_result = subprocess.run(
                                    [exe_file],
                                    stdin=stdin_file,
                                    capture_output=True,
                                    text=True,
                                    timeout=10
                                )
                        else:
                            run_result = subprocess.run(
                                [exe_file],
                                capture_output=True,
                                text=True,
                                timeout=10
                            )
                        
                        response = {
                            "stdout": run_result.stdout,
                            "stderr": run_result.stderr,
                            "exit_code": run_result.returncode
                        }
                    
                    # Cleanup C and executable files
                    if os.path.exists(c_file):
                        os.unlink(c_file)
                    if os.path.exists(exe_file):
                        os.unlink(exe_file)
                
            except FileNotFoundError:
                response = {
                    "stdout": "",
                    "stderr": f"ERROR: Kotha compiler not found at: {KOTHA_BIN}\n\nPlease run:\n  cd kotha\n  make clean && make\n",
                    "exit_code": 127
                }
            except subprocess.TimeoutExpired:
                response = {
                    "stdout": "",
                    "stderr": "ERROR: Execution timeout (10 seconds)",
                    "exit_code": 124
                }
            except Exception as e:
                response = {
                    "stdout": "",
                    "stderr": f"ERROR: {str(e)}",
                    "exit_code": 1
                }
            finally:
                if os.path.exists(tmp_path):
                    os.unlink(tmp_path)
                if input_file and os.path.exists(input_file):
                    os.unlink(input_file)

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(response).encode('utf-8'))
        else:
            self.send_error(404)

print("=" * 60)
print("🚀 Kotha IDE Server Starting...")
print("=" * 60)
print(f"📝 URL: http://localhost:{PORT}")
print(f"🔧 Compiler: {KOTHA_BIN}")
print(f"📁 Files: {os.getcwd()}")

# Check if compiler exists
if os.path.exists(KOTHA_BIN):
    print("✅ Compiler found!")
else:
    print("⚠️  WARNING: Compiler not found!")
    print("   Please run: cd kotha && make")

print("=" * 60)
print("")

with socketserver.TCPServer(("", PORT), KothaHandler) as httpd:
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n\n👋 Server stopped")
