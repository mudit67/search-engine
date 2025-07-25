# 📄 PDF Search Engine

A high-performance PDF Search Engine that parses and indexes PDF files for efficient, scalable search and retrieval. Built with a focus on speed, accuracy, and usability.

---

## 🚀 Features

### 🔍 DF Parsing & Text Extraction
- Converts PDF documents into clean, readable plain text.
- Utilizes a robust PDF parser to handle multi-page and complex PDFs.

### 🧹 Text Preprocessing
- Tokenizes and normalizes extracted text.
- Removes punctuation, stopwords, and extraneous whitespace for cleaner indexing.

### 🧠 Indexing Mechanism
- Uses optimized data structures (e.g., inverted indices).
- Enables fast and scalable search across large document collections.

### 📝 Query Handling & Search
- Accepts user queries via a simple interface.
- Retrieves relevant matches using optimized search algorithms.

### 📊 Result Ranking & Display
- Applies TF-IDF for relevance-based ranking.
- Displays results in an intuitive, user-friendly format.

---

## ⚙️ Tech Stack

- **Language**: C++  
- **PDF Parsing**: Poppler / PDFium  
- **Data Structures**: Inverted Index, Hash Maps  
- **Ranking**: TF-IDF  
- **Search**: Boolean / Fuzzy Matching  
- **Interface**: Console-based (CLI) or Web UI (optional)

---

## 📁 Project Structure
```
Search-engine/
├── include/ # Header files
├── assets/ # Usable files
├── src/ # Source code
│ ├── parser.cpp # PDF text extraction
│ ├── tokenizer.cpp # Text preprocessing
│ ├── index.cpp # Inverted index creation
│ ├── search.cpp # Query parsing and result retrieval
│ └── main.cpp # Entry point
├── tests/ # Unit and integration tests
└── README.md
```
### Clone the Repository
```bash
git clone https://github.com/mudit67/search-engine.git
cd search-engine
