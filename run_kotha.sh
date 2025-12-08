#!/bin/bash
# Kotha Demo Helper Script
# This script will help you run Kotha commands easily

echo "🐯 Kotha Demo Helper"
echo "===================="
echo ""

# Navigate to kotha directory
cd /Users/aupurbasarker/Documents/Kotha0.2/kotha

# Make executable
chmod +x kotha

echo "✅ Ready! You can now run:"
echo ""
echo "  ./kotha version"
echo "  ./kotha help"
echo "  ./kotha run test_simple_vm.kotha"
echo "  ./kotha repl"
echo ""
echo "Current directory: $(pwd)"
echo ""

# Run version command
echo "Running: ./kotha version"
echo "------------------------"
./kotha version
