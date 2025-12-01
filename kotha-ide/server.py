import http.server
import socketserver
import subprocess
import os
import json
import tempfile

PORT = 8080
# Path to the kotha-cli script
KOTHA_CLI = os.path.abspath(os.path.join(os.path.dirname(__file__), "../kotha/kotha-cli"))

# Change to the directory where server.py is located
os.chdir(os.path.dirname(os.path.abspath(__file__)))

class KothaHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/":
            self.path = "/public/index.html"
        elif self.path.startswith("/public/"):
            pass # Serve as is
        else:
            self.path = "/public" + self.path
        return http.server.SimpleHTTPRequestHandler.do_GET(self)

    def do_POST(self):
        if self.path == "/api/run":
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            code = data.get('code', '')

            # Create temp file
            with tempfile.NamedTemporaryFile(mode='w', suffix='.kotha', delete=False) as tmp:
                tmp.write(code)
                tmp_path = tmp.name

            try:
                # Run compiler
                result = subprocess.run(
                    [KOTHA_CLI, "run", tmp_path],
                    capture_output=True,
                    text=True,
                    cwd=os.path.dirname(KOTHA_CLI)
                )
                
                response = {
                    "stdout": result.stdout,
                    "stderr": result.stderr,
                    "exit_code": result.returncode
                }
            except Exception as e:
                response = {
                    "stdout": "",
                    "stderr": str(e),
                    "exit_code": 1
                }
            finally:
                if os.path.exists(tmp_path):
                    os.unlink(tmp_path)

            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(json.dumps(response).encode('utf-8'))
        else:
            self.send_error(404)

print(f"Serving Kotha IDE at http://localhost:{PORT}")
print(f"Using compiler at: {KOTHA_CLI}")
print(f"Serving files from: {os.getcwd()}")

with socketserver.TCPServer(("", PORT), KothaHandler) as httpd:
    httpd.serve_forever()
