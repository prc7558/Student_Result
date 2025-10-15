// ==================== BACKEND-CONNECTED VERSION ====================
// This version connects to C++ backend via Node.js bridge

const API_URL = 'http://localhost:3000/api';
let courseCount = 1;
let classmateData = [];

// ==================== TAB SWITCHING ====================
document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', function() {
        const tabName = this.dataset.tab;
        
        document.querySelectorAll('.tab-btn').forEach(t => t.classList.remove('active'));
        document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));
        
        this.classList.add('active');
        document.getElementById(tabName).classList.add('active');
        
        if (tabName === 'classmate') {
            loadClassmateData();
        } else if (tabName === 'view') {
            loadAllResults();
        }
    });
});

// ==================== ADD COURSE FUNCTION ====================
function addCourse() {
    courseCount++;
    const coursesContainer = document.getElementById('coursesContainer');
    
    const courseCard = document.createElement('div');
    courseCard.className = 'course-card';
    courseCard.innerHTML = `
        <div class="course-header">
            <h4>Course ${courseCount}</h4>
            <button type="button" class="btn-remove" onclick="removeCourse(this)" style="background: #ef4444; color: white; border: none; padding: 8px 16px; border-radius: 6px; cursor: pointer;">✕ Remove</button>
        </div>
        <div class="form-grid">
            <div class="form-group">
                <label>Course Code</label>
                <input type="text" class="courseCode" placeholder="e.g., CSE101" required>
            </div>
            <div class="form-group">
                <label>Course Name</label>
                <input type="text" class="courseName" placeholder="e.g., Data Structures" required>
            </div>
            <div class="form-group">
                <label>Marks Obtained</label>
                <input type="number" class="courseMarks" min="0" placeholder="Marks" required>
            </div>
            <div class="form-group">
                <label>Maximum Marks</label>
                <input type="number" class="courseMaxMarks" min="1" placeholder="Max Marks" required>
            </div>
        </div>
    `;
    
    coursesContainer.appendChild(courseCard);
    showToast('Course added successfully!', 'success');
}

// ==================== REMOVE COURSE ====================
function removeCourse(btn) {
    if (courseCount > 1) {
        btn.closest('.course-card').remove();
        courseCount--;
        showToast('Course removed', 'warning');
    } else {
        showToast('At least one course is required!', 'error');
    }
}

// ==================== ADD STUDENT (BACKEND) ====================
document.getElementById('addForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    
    const prn = document.getElementById('prn').value.trim().toUpperCase();
    const studentName = document.getElementById('studentName').value.trim();
    
    const courseCards = document.querySelectorAll('.course-card');
    const courses = [];
    
    courseCards.forEach(card => {
        const code = card.querySelector('.courseCode').value.trim();
        const name = card.querySelector('.courseName').value.trim();
        const marks = parseFloat(card.querySelector('.courseMarks').value);
        const maxMarks = parseFloat(card.querySelector('.courseMaxMarks').value);
        
        courses.push({ code, name, marks, maxMarks });
    });
    
    // Show loading
    showLoading(true);
    
    try {
        const response = await fetch(`${API_URL}/add-student`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                prn,
                name: studentName,
                courses
            })
        });
        
        const result = await response.json();
        
        if (result.success) {
            displayResult(result.data);
            showToast('✓ Student result saved to C++ backend!', 'success');
            
            // Reset form
            this.reset();
            courseCount = 1;
            document.getElementById('coursesContainer').innerHTML = getDefaultCourseCard();
        } else {
            showToast('✗ Error: ' + result.error, 'error');
        }
    } catch (error) {
        console.error('Error:', error);
        showToast('✗ Failed to connect to backend!', 'error');
    } finally {
        showLoading(false);
    }
});

// ==================== GET DEFAULT COURSE CARD ====================
function getDefaultCourseCard() {
    return `
        <div class="course-card">
            <div class="course-header">
                <h4>Course 1</h4>
            </div>
            <div class="form-grid">
                <div class="form-group">
                    <label>Course Code</label>
                    <input type="text" class="courseCode" placeholder="e.g., CSE101" required>
                </div>
                <div class="form-group">
                    <label>Course Name</label>
                    <input type="text" class="courseName" placeholder="e.g., Data Structures" required>
                </div>
                <div class="form-group">
                    <label>Marks Obtained</label>
                    <input type="number" class="courseMarks" min="0" placeholder="Marks" required>
                </div>
                <div class="form-group">
                    <label>Maximum Marks</label>
                    <input type="number" class="courseMaxMarks" min="1" placeholder="Max Marks" required>
                </div>
            </div>
        </div>
    `;
}

// ==================== DISPLAY RESULT ====================
function displayResult(student) {
    const resultHTML = `
        <div class="result-card">
            <h3><span class="icon">📋</span> Result Calculated Successfully!</h3>
            <p style="color: #10b981; font-weight: 600; margin: 10px 0;">✓ Saved to C++ Backend (reportcards.txt)</p>
            <div class="result-info">
                <div class="info-item">
                    <div class="info-label">PRN Number</div>
                    <div class="info-value">${student.prn}</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Student Name</div>
                    <div class="info-value">${student.name}</div>
                </div>
                <div class="info-item">
                    <div class="info-label">Percentage</div>
                    <div class="info-value">${student.percentage.toFixed(2)}%</div>
                </div>
            </div>
            
            <table class="course-table">
                <thead>
                    <tr>
                        <th>Course Code</th>
                        <th>Course Name</th>
                        <th>Marks</th>
                        <th>Max Marks</th>
                    </tr>
                </thead>
                <tbody>
                    ${student.courses.map(c => `
                        <tr>
                            <td>${c.code}</td>
                            <td>${c.name}</td>
                            <td>${c.marks}</td>
                            <td>${c.maxMarks}</td>
                        </tr>
                    `).join('')}
                </tbody>
            </table>
            
            <div class="grade-display grade-${student.grade}">
                <h2>Grade: ${student.grade}</h2>
                <p>${getGradeMessage(student.grade)}</p>
            </div>
        </div>
    `;
    
    const formCard = document.querySelector('#add .card-body');
    const existingResult = formCard.querySelector('.result-card');
    if (existingResult) {
        existingResult.remove();
    }
    formCard.insertAdjacentHTML('beforeend', resultHTML);
}

// ==================== GET GRADE MESSAGE ====================
function getGradeMessage(grade) {
    const messages = {
        'A': '🌟 Excellent Performance!',
        'B': '✨ Very Good!',
        'C': '👍 Good Job!',
        'D': '📚 Keep Working Hard!',
        'E': '💪 Need More Effort!',
        'F': '⚠️ Failed - Study Harder!'
    };
    return messages[grade] || '';
}

// ==================== SEARCH STUDENT (BACKEND) ====================
document.getElementById('searchForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    
    const searchPRN = document.getElementById('searchPRN').value.trim().toUpperCase();
    const resultDiv = document.getElementById('searchResult');
    
    showLoading(true);
    
    try {
        const response = await fetch(`${API_URL}/search-student/${searchPRN}`);
        const result = await response.json();
        
        if (result.success) {
            const student = result.data;
            resultDiv.innerHTML = `
                <div class="result-card">
                    <h3><span class="icon">✓</span> Student Found!</h3>
                    <div class="result-info">
                        <div class="info-item">
                            <div class="info-label">PRN Number</div>
                            <div class="info-value">${student.prn}</div>
                        </div>
                        <div class="info-item">
                            <div class="info-label">Student Name</div>
                            <div class="info-value">${student.name}</div>
                        </div>
                        <div class="info-item">
                            <div class="info-label">Percentage</div>
                            <div class="info-value">${student.percentage.toFixed(2)}%</div>
                        </div>
                    </div>
                    
                    <table class="course-table">
                        <thead>
                            <tr>
                                <th>Course Code</th>
                                <th>Course Name</th>
                                <th>Marks</th>
                                <th>Max Marks</th>
                            </tr>
                        </thead>
                        <tbody>
                            ${student.courses.map(c => `
                                <tr>
                                    <td>${c.code}</td>
                                    <td>${c.name}</td>
                                    <td>${c.marks}</td>
                                    <td>${c.maxMarks}</td>
                                </tr>
                            `).join('')}
                        </tbody>
                    </table>
                    
                    <div class="grade-display grade-${student.grade}">
                        <h2>Grade: ${student.grade}</h2>
                        <p>${getGradeMessage(student.grade)}</p>
                    </div>
                </div>
            `;
            showToast('✓ Student record found!', 'success');
        } else {
            resultDiv.innerHTML = `
                <div class="result-card" style="background: linear-gradient(135deg, #fee2e2 0%, #fecaca 100%); border-color: #fca5a5;">
                    <h3 style="color: #dc2626;"><span class="icon">✗</span> Student Not Found</h3>
                    <p style="color: #991b1b; margin-top: 10px;">No record found for PRN: <strong>${searchPRN}</strong></p>
                </div>
            `;
            showToast('✗ Student not found!', 'error');
        }
    } catch (error) {
        console.error('Error:', error);
        resultDiv.innerHTML = `
            <div class="result-card" style="background: linear-gradient(135deg, #fee2e2 0%, #fecaca 100%);">
                <h3 style="color: #dc2626;">✗ Backend Connection Error</h3>
                <p style="color: #991b1b; margin-top: 10px;">Make sure the server is running!</p>
            </div>
        `;
        showToast('✗ Failed to connect to backend!', 'error');
    } finally {
        showLoading(false);
    }
});

// ==================== SEARCH CLASSMATE (BACKEND) ====================
document.getElementById('classmateSearchForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    
    const searchPRN = document.getElementById('classmatePRN').value.trim().toUpperCase();
    const resultDiv = document.getElementById('classmateResult');
    
    showLoading(true);
    
    try {
        const response = await fetch(`${API_URL}/search-classmate/${searchPRN}`);
        const result = await response.json();
        
        if (result.success) {
            resultDiv.innerHTML = `
                <div class="result-card">
                    <h3><span class="icon">✓</span> Classmate Found!</h3>
                    <div class="result-info">
                        <div class="info-item">
                            <div class="info-label">PRN Number</div>
                            <div class="info-value">${result.prn}</div>
                        </div>
                        <div class="info-item">
                            <div class="info-label">Percentage</div>
                            <div class="info-value">${result.percentage.toFixed(2)}%</div>
                        </div>
                    </div>
                    
                    <div class="grade-display grade-${result.grade}">
                        <h2>Grade: ${result.grade}</h2>
                        <p>${getGradeMessage(result.grade)}</p>
                    </div>
                </div>
            `;
            showToast('✓ Classmate record found!', 'success');
        } else {
            resultDiv.innerHTML = `
                <div class="result-card" style="background: linear-gradient(135deg, #fee2e2 0%, #fecaca 100%);">
                    <h3 style="color: #dc2626;">✗ Classmate Not Found</h3>
                    <p style="color: #991b1b; margin-top: 10px;">No record found for PRN: <strong>${searchPRN}</strong></p>
                </div>
            `;
            showToast('✗ Classmate not found!', 'error');
        }
    } catch (error) {
        console.error('Error:', error);
        showToast('✗ Failed to connect to backend!', 'error');
    } finally {
        showLoading(false);
    }
});

// ==================== LOAD CLASSMATE DATA ====================
function loadClassmateData() {
    // Statistics from CSV
    const csvData = `B24CE1001,Tushar Borate,64
B24CE1002,Sahil Veer,77
B24CE1003,Vishwesh Bhilare,83
B24CE1004,Abhijit Bedse,46
B24CE1005,Pravin Ambhore,68
B24CE1006,Avishkar Patil,62
B24CE1007,Neel Kadam,59
B24CE1008,Kshitij Bhate,57
B24CE1009,Mahesh Gaikwad,69
B24CE1010,Pooja Umare,43
B24CE1011,Ayush Chandekar,31
B24CE1012,Ritik Mirase,53
B24CE1013,Vedantika Ranaware,95
B24CE1014,Rugved Deshwant,81
B24CE1015,Swarada Deshpande,68
B24CE1016,Jahnavi Dhanapune,65
B24CE1017,Vaidehi Shinde,84
B24CE1018,Yugant Varekar,83
B24CE1019,Sajiree Damle,92
B24CE1020,Yash More,36
B24CE1021,Gayatri Dethe,54
B24CE1022,Srushti Shewale,55
B24CE1023,Shravani Pataskar,91
B24CE1024,Jidnyasa Patil,93
B24CE1025,Ashutosh Kulkarni,74
B24CE1026,Balraj Rasal,50
B24CE1027,Anula Borse,52
B24CE1028,Gunaji Mohite,68
B24CE1029,Altamash Mujawar,81
B24CE1030,Rajkumar Marathe,77
B24CE1031,Ashlesha Humne,76
B24CE1032,Vikrant Dagadkhair,53
B24CE1033,Jayant Raut,68
B24CE1034,Abhijeet Hake,81
B24CE1035,Dnyaneshwari Thorat,80
B24CE1036,Arihant Borge,56
B24CE1037,Pruthviraj Kokate,64
B24CE1038,Raj Mohale,69
B24CE1039,Prathamesh Kirtane,11
B24CE1040,Soham Badarkhe,64
B24CE1041,Shivtej Wagare,50
B24CE1042,Shubham Dorake,55
B24CE1043,Sanskruti Gudhe,50
B24CE1044,Tanvi Kavate,4
B24CE1045,Soham Gawde,65
B24CE1046,Parth Kshirsagar,53
B24CE1047,Anurag Patil,79
B24CE1048,Rohan Mali,49
B24CE1049,Isha Patil,83
B24CE1050,Parth Chaudhari,94
B24CE1051,Khilesh Chaudhari,93
B24CE1052,Yagnesh Paliwal,88
B24CE1053,Animish Deo,68
B24CE1054,Asmi Tatawar,76
B24CE1055,Niraj Fegade,72
B24CE1056,Janhavi Patil,87
B24CE1057,Om Chandurkar,64
B24CE1058,Atharva Bhonde,43
B24CE1059,Srujal Bhonde,34
B24CE1060,Parth Topale,49
B24CE1127,Niranjan Mule,53
B24CE1128,Ritesh Manthole,76
B24CE1129,Krisha Kulkarni,81
B24CE1130,Aayush Joshi,62
B24CE1132,Renuka Kulkarni,57
B24CE1139,Divyansh Rai,19`;

    classmateData = csvData.split('\n').map(line => {
        const [prn, name, percentage] = line.split(',');
        return { prn, name, percentage: parseFloat(percentage) };
    });
    
    displayClassmateStats();
}

// ==================== DISPLAY CLASSMATE STATISTICS ====================
function displayClassmateStats() {
    if (classmateData.length === 0) return;
    
    const percentages = classmateData.map(s => s.percentage);
    const total = classmateData.length;
    const average = (percentages.reduce((a, b) => a + b, 0) / total).toFixed(2);
    const highest = Math.max(...percentages);
    const lowest = Math.min(...percentages);
    const passed = classmateData.filter(s => s.percentage >= 40).length;
    
    const statsHTML = `
        <div class="stat-card" style="background: linear-gradient(135deg, #10b981, #059669);">
            <div class="stat-value">${total}</div>
            <div class="stat-label">Total Students</div>
        </div>
        <div class="stat-card" style="background: linear-gradient(135deg, #3b82f6, #2563eb);">
            <div class="stat-value">${average}%</div>
            <div class="stat-label">Class Average</div>
        </div>
        <div class="stat-card" style="background: linear-gradient(135deg, #f59e0b, #d97706);">
            <div class="stat-value">${highest}%</div>
            <div class="stat-label">Highest Score</div>
        </div>
        <div class="stat-card" style="background: linear-gradient(135deg, #ef4444, #dc2626);">
            <div class="stat-value">${lowest}%</div>
            <div class="stat-label">Lowest Score</div>
        </div>
        <div class="stat-card" style="background: linear-gradient(135deg, #8b5cf6, #7c3aed);">
            <div class="stat-value">${passed}</div>
            <div class="stat-label">Students Passed</div>
        </div>
        <div class="stat-card" style="background: linear-gradient(135deg, #ec4899, #db2777);">
            <div class="stat-value">${total - passed}</div>
            <div class="stat-label">Students Failed</div>
        </div>
    `;
    
    document.getElementById('classmateStats').innerHTML = statsHTML;
}

// ==================== LOAD ALL RESULTS (BACKEND) ====================
async function loadAllResults() {
    const resultsDiv = document.getElementById('allResults');
    showLoading(true);
    
    try {
        const response = await fetch(`${API_URL}/all-students`);
        const result = await response.json();
        
        if (result.success && result.data.length > 0) {
            resultsDiv.innerHTML = result.data.map(student => `
                <div class="result-card">
                    <h3 style="color: var(--primary); margin-bottom: 15px;">${student.name}</h3>
                    <div class="result-info">
                        <div class="info-item">
                            <div class="info-label">PRN</div>
                            <div class="info-value" style="font-size: 1rem;">${student.prn}</div>
                        </div>
                        <div class="info-item">
                            <div class="info-label">Percentage</div>
                            <div class="info-value">${student.percentage.toFixed(2)}%</div>
                        </div>
                    </div>
                    <div class="grade-display grade-${student.grade}" style="margin-top: 15px; padding: 15px;">
                        <h2 style="font-size: 2rem; margin-bottom: 5px;">Grade: ${student.grade}</h2>
                    </div>
                </div>
            `).join('');
        } else {
            resultsDiv.innerHTML = `
                <div class="result-card" style="grid-column: 1 / -1;">
                    <h3 style="color: #64748b; text-align: center;"><span class="icon">📭</span> No Results Found</h3>
                    <p style="color: #64748b; text-align: center; margin-top: 10px;">Add some student results to see them here.</p>
                </div>
            `;
        }
    } catch (error) {
        console.error('Error:', error);
        resultsDiv.innerHTML = `
            <div class="result-card" style="grid-column: 1 / -1;">
                <h3 style="color: #dc2626; text-align: center;">✗ Backend Connection Error</h3>
                <p style="color: #991b1b; text-align: center; margin-top: 10px;">Make sure the server is running!</p>
            </div>
        `;
    } finally {
        showLoading(false);
    }
}

// ==================== SHOW/HIDE LOADING ====================
function showLoading(show) {
    const loading = document.getElementById('loading');
    if (show) {
        loading.style.display = 'flex';
    } else {
        loading.style.display = 'none';
    }
}

// ==================== TOAST NOTIFICATION ====================
function showToast(message, type = 'success') {
    const toast = document.getElementById('toast');
    toast.textContent = message;
    toast.className = `toast ${type} show`;
    
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

// ==================== INITIALIZE ON PAGE LOAD ====================
document.addEventListener('DOMContentLoaded', function() {
    loadClassmateData();
    console.log('╔════════════════════════════════════════════════╗');
    console.log('║   Student Result Management System (Backend)   ║');
    console.log('╚════════════════════════════════════════════════╝');
    console.log('✓ Connected to C++ Backend via Node.js Bridge');
    console.log('✓ OOP Concepts: Inheritance, Encapsulation');
    console.log('✓ File Handling: reportcards.txt');
    console.log('✓ STL Map: CSV search functionality');
    console.log('✓ Ready for use!');
});