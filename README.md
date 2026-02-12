# BinaryStar 🌟

**BinaryStar** is a secure Two-Party Computation (2PC) framework for the web.

It allows users to perform calculations on private data without revealing that data to the servers. Data is split in the browser, processed blindly by two isolated nodes (Alpha & Beta), and reconstructed only on the user's device.

## 🚀 How it Works

1.  **Split**: The browser splits input data into random shares ($x = x_1 + x_2$).
2.  **Distribute**: Share 1 is sent to **Node A**, and Share 2 is sent to **Node B**.
3.  **Blind Compute**: Nodes compute the result using MPC protocols (e.g., OT, Beaver Triples) without ever seeing the original data.
4.  **Return**: Nodes send the *result shares* back to the browser. They do not reconstruct the data.
5.  **Merge**: The browser combines the shares to display the final result.

## 📂 Project Structure

- **`core/`** (C++): The high-performance MPC nodes (Alpha & Beta) based on the SCI library.
- **`coordinator/`** (Go): A lightweight server that handles WebSocket connections and signaling.
- **`web/`** (TypeScript): The frontend client responsible for secret sharing and reconstruction.

## 🛠️ Getting Started

### Prerequisites
- Docker & Docker Compose

### Quick Start

The easiest way to run the full system (Coordinator + 2 Nodes + Web) is using Docker.

```bash
# Clone the repository
git clone [https://github.com/your-username/binary-star.git](https://github.com/your-username/binary-star.git)
cd binary-star

# Start all services
docker-compose up --build