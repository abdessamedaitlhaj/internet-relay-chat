#!/bin/bash

# First, create a script to run the server
cat > run_server.sh << 'EOF'
#!/bin/bash
./ircserv 4444 pass
EOF

# Create a script to run each client
cat > run_client.sh << 'EOF'
#!/bin/bash
nc localhost 4444
EOF

# Make the scripts executable
chmod +x run_server.sh run_client.sh

# Open terminal with server (using gnome-terminal, kitty, or iTerm2 depending on your system)

if command -v osascript &> /dev/null; then
    # For macOS
    osascript -e 'tell app "Terminal" to do script "'"$PWD/run_server.sh"'"'
    sleep 1
    osascript -e 'tell app "Terminal" to do script "'"$PWD/run_client.sh"'"'
    osascript -e 'tell app "Terminal" to do script "'"$PWD/run_client.sh"'"'
else
    echo "No supported terminal emulator found"
    exit 1
fi