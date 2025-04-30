# 🧪 MiniGit – Lightweight Version Control System

**MiniGit** is a simplified version control prototype inspired by Git.  
It supports file hashing, snapshot creation (blobs), versioned tree structures, and full file system state backup and restoration.

---

## 🛠️ Key Features

| Feature                  | Description |
|--------------------------|-------------|
| 🔐 **File Hashing**       | Computes SHA-256 hashes to uniquely identify file contents |
| 📦 **Blob Management**     | Creates unique file snapshots stored in a `XX/YY...` directory structure based on their hash |
| 🌳 **Advanced Structures** | Uses `WorkFile` and `WorkTree` structures to manage versioned states |
| 💾 **Backup & Restore**    | Saves and restores the entire file system state at any given moment |

---

## 🧩 Technical Components

| File        | Purpose |
|-------------|---------|
| `hash.c`    | Manages SHA-256 hashing and the physical storage of files |
| `list.c`    | Implements a linked list for browsing directories |
| `work.c`    | Handles the `WorkFile` and `WorkTree` structures used in versioning |
| `main.c`    | Contains tests and an example usage of the system |

---

## 🎯 Educational Objective

This project serves as a **tester** to verify that your system can:

- ✅ Compute SHA-256 hashes of files  
- ✅ Traverse directories and manage file lists  
- ✅ Create and store file snapshots (blobs)  
- ✅ Organize versioned file trees (WorkTree)  
- ✅ Backup and restore full project states

> It is the **first step** toward building a real version control tool like Git.

---

## 🐧 Platform Compatibility

This project is designed to run on **Linux-based systems** (e.g., Ubuntu, Debian).  
It uses POSIX system calls and standard Unix directory management.

---

## 🖥️ How to Compile and Run (Linux)

```bash

# 1. Open a terminal and navigate to the project directory:

cd /path/to/your/project

# 2. Compile the source files using gcc:

gcc -o project main.c hash.c list.c work.c

# 3. Execute the program:

./project

# ✅ Make sure gcc is installed:

sudo apt install build-essential
```

---

## 📁 Project Structure 

MiniGit/
├── hash.c
├── list.c
├── work.c
├── main.c
├── hash.h
├── list.h
├── work.h
└── README.md



