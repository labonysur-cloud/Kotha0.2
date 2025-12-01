document.addEventListener('DOMContentLoaded', () => {
    const btnRun = document.getElementById('btn-run');
    const btnNew = document.getElementById('btn-new');
    const btnSave = document.getElementById('btn-save');
    const codeEditor = document.getElementById('code-editor');
    const consoleOutput = document.getElementById('console-output');
    
    let currentFilename = 'untitled.kotha';

    // Run button
    btnRun.addEventListener('click', async () => {
        const code = codeEditor.value;
        
        if (!code.trim()) {
            consoleOutput.textContent = "Error: No code to run!";
            return;
        }
        
        consoleOutput.textContent = "🐯 Compiling and running...\n";
        btnRun.disabled = true;
        
        try {
            const response = await fetch('/api/run', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({ code })
            });

            const data = await response.json();
            
            let output = "";
            if (data.exit_code === 0) {
                output += "--- SUCCESS ---\n";
                output += data.stdout;
            } else {
                output += "--- ERROR ---\n";
                output += data.stderr;
                if (data.stdout) {
                    output += "\n--- STDOUT ---\n" + data.stdout;
                }
            }
            
            consoleOutput.textContent = output;
        } catch (error) {
            consoleOutput.textContent = "Error connecting to server: " + error.message;
        } finally {
            btnRun.disabled = false;
        }
    });

    // New button
    btnNew.addEventListener('click', () => {
        if (codeEditor.value.trim() && !confirm('Create new file? Unsaved changes will be lost.')) {
            return;
        }
        
        codeEditor.value = `main function {
    dhoro x = 10;
    dhoro y = 20;
    dekhaw("Hello from Kotha!");
    dekhaw(x + y);
}`;
        currentFilename = 'untitled.kotha';
        updateTitle();
        consoleOutput.textContent = "New file created. Ready to code! 🐯";
    });

    // Save button
    btnSave.addEventListener('click', () => {
        const code = codeEditor.value;
        const blob = new Blob([code], { type: 'text/plain' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = currentFilename;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
        consoleOutput.textContent = `File saved as ${currentFilename} 💾`;
    });

    // Update window title
    function updateTitle() {
        document.querySelector('.title-bar-text').textContent = `Kotha IDE - ${currentFilename}`;
    }

    // Keyboard shortcuts
    document.addEventListener('keydown', (e) => {
        // Ctrl/Cmd + S to save
        if ((e.ctrlKey || e.metaKey) && e.key === 's') {
            e.preventDefault();
            btnSave.click();
        }
        // Ctrl/Cmd + Enter to run
        if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
            e.preventDefault();
            btnRun.click();
        }
        // Ctrl/Cmd + N for new
        if ((e.ctrlKey || e.metaKey) && e.key === 'n') {
            e.preventDefault();
            btnNew.click();
        }
    });

    // Add example snippets to menu
    const helpMenu = document.querySelector('.menu-bar span:last-child');
    helpMenu.addEventListener('click', () => {
        const examples = [
            {
                name: 'Hello World',
                code: `main function {
    dekhaw("Hello, World!");
}`
            },
            {
                name: 'Variables & Constants',
                code: `main function {
    sthir PI = 3;
    dhoro radius = 5;
    dhoro area = PI * radius * radius;
    dekhaw("Area:");
    dekhaw(area);
}`
            },
            {
                name: 'If-Else',
                code: `main function {
    dhoro age = 20;
    
    jodi (age < 13) {
        dekhaw("Child");
    } noyto (age < 20) {
        dekhaw("Teenager");
    } othoba {
        dekhaw("Adult");
    }
}`
            },
            {
                name: 'Loops',
                code: `main function {
    cholbe (i theke 1 porjonto 5) {
        dekhaw(i);
    }
}`
            }
        ];
        
        let choice = prompt('Choose example:\n1. Hello World\n2. Variables & Constants\n3. If-Else\n4. Loops\n\nEnter number (1-4):');
        if (choice && choice >= 1 && choice <= 4) {
            codeEditor.value = examples[choice - 1].code;
            currentFilename = examples[choice - 1].name.toLowerCase().replace(/\s+/g, '_') + '.kotha';
            updateTitle();
            consoleOutput.textContent = `Loaded example: ${examples[choice - 1].name}`;
        }
    });

    // Initial message
    consoleOutput.textContent = "Kotha IDE Ready! 🐯\n\nKeyboard Shortcuts:\n- Ctrl/Cmd + Enter: Run\n- Ctrl/Cmd + S: Save\n- Ctrl/Cmd + N: New\n\nClick Help for examples!";
});
