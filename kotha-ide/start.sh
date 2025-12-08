#!/bin/bash
# Start Kotha IDE Server

echo "Starting Kotha IDE Server..."
echo ""

cd "$(dirname "$0")"

# Kill any existing server on port 8080
lsof -ti:8080 | xargs kill -9 2>/dev/null || true
sleep 1

# Start the simplified server
python3 server_simple.py
