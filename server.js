// ==================== NODE.JS BRIDGE SERVER ====================
// This server acts as a bridge between your website and C++ program

const express = require('express');
const { spawn } = require('child_process');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = 3000;

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.static(__dirname)); // Serve HTML, CSS, JS files

// Path to your C++ executable
const CPP_EXECUTABLE = path.join(__dirname, 'Student_Result_Management_Enhanced.exe');

// ==================== HELPER FUNCTION ====================
function runCppCommand(command) {
    return new Promise((resolve, reject) => {
        const child = spawn(CPP_EXECUTABLE, ['--web']);
        
        let output = '';
        let errorOutput = '';
        
        child.stdout.on('data', (data) => {
            output += data.toString();
        });
        
        child.stderr.on('data', (data) => {
            errorOutput += data.toString();
        });
        
        child.on('close', (code) => {
            if (code !== 0) {
                console.error('C++ Error:', errorOutput);
                reject(new Error(errorOutput || 'C++ program exited with error'));
                return;
            }
            resolve(output.trim());
        });
        
        child.on('error', (err) => {
            console.error('Spawn Error:', err);
            reject(err);
        });
        
        // Send command to C++ program via stdin
        child.stdin.write(command + '\n');
        child.stdin.end();
    });
}

// ==================== API ENDPOINTS ====================

// Add new student result
app.post('/api/add-student', async (req, res) => {
    try {
        const { prn, name, courses } = req.body;
        
        // Format: ADD|PRN|Name|CourseCount|Code1|Name1|Marks1|Max1|...
        let command = `ADD|${prn}|${name}|${courses.length}`;
        courses.forEach(c => {
            command += `|${c.code}|${c.name}|${c.marks}|${c.maxMarks}`;
        });
        
        console.log('Command:', command);
        const result = await runCppCommand(command);
        console.log('C++ Output:', result);
        
        const studentData = JSON.parse(result);
        
        res.json({ success: true, data: studentData });
    } catch (error) {
        console.error('Add student error:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// Search student by PRN
app.get('/api/search-student/:prn', async (req, res) => {
    try {
        const prn = req.params.prn;
        const command = `SEARCH|${prn}`;
        
        console.log('Command:', command);
        const result = await runCppCommand(command);
        console.log('C++ Output:', result);
        
        const studentData = JSON.parse(result);
        
        if (studentData.error) {
            res.status(404).json({ success: false, error: studentData.error });
        } else {
            res.json({ success: true, data: studentData });
        }
    } catch (error) {
        console.error('Search student error:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// Search classmate percentage
app.get('/api/search-classmate/:prn', async (req, res) => {
    try {
        const prn = req.params.prn;
        const command = `CLASSMATE|${prn}`;
        
        console.log('Command:', command);
        const result = await runCppCommand(command);
        console.log('C++ Output:', result);
        
        const classmateData = JSON.parse(result);
        
        res.json(classmateData);
    } catch (error) {
        console.error('Search classmate error:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// Get all saved results
app.get('/api/all-students', async (req, res) => {
    try {
        const command = 'GET_ALL';
        
        console.log('Command:', command);
        const result = await runCppCommand(command);
        console.log('C++ Output:', result);
        
        const students = JSON.parse(result);
        
        res.json({ success: true, data: students });
    } catch (error) {
        console.error('Get all students error:', error);
        res.status(500).json({ success: false, error: error.message });
    }
});

// ==================== START SERVER ====================
app.listen(PORT, () => {
    console.log('╔════════════════════════════════════════════════╗');
    console.log('║   Student Result Management System Server      ║');
    console.log('╚════════════════════════════════════════════════╝');
    console.log(`✓ Server running on http://localhost:${PORT}`);
    console.log(`✓ C++ Backend: ${CPP_EXECUTABLE}`);
    console.log(`✓ Ready to handle requests!`);
    console.log('\n🌐 Open http://localhost:3000 in your browser');
    console.log('📝 Make sure the C++ .exe file is compiled and in the same folder\n');
});