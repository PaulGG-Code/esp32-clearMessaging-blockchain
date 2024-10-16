
# ESP32 Blockchain Message System

This project demonstrates how to register an ESP32 device on a blockchain, using a backend server written in Node.js. The device send  messages to other registered devices through the blockchain.

## Features

- **ESP32 Device Registration:** Each ESP32 device generates device ID and registers with the backend.
- **Blockchain Integration:** The backend registers devices on the blockchain using a smart contract.
- **Message Encryption:** Devices can send  messages to each other over the blockchain.
- **Device Listing:** The backend can list all registered devices, allowing the selection of a recipient for messages.

## Prerequisites

### Hardware

- ESP32 board (e.g., TTGO T-Display, ESP32 Dev Kit, etc.)
- Wi-Fi network for the ESP32 to connect to

### Software

- Node.js (>= 18.x.x)
- NPM or Yarn
- Ethers.js library (v6.x)
- Arduino IDE for ESP32 programming (or PlatformIO)
- A blockchain network supporting smart contracts (e.g., Arbitrum Sepolia)

## Smart Contract

The project uses a simple smart contract deployed on an Ethereum-compatible blockchain. The contract allows for device registration, storing public keys, and sending encrypted messages between devices.

### Contract ABI

Make sure to deploy a smart contract on the blockchain, and include its ABI and address in the backend code:

```javascript
const contractAddress = "<your-contract-address>";
const abi = [
// ABI goes here
];
```

## Setup

### Backend Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/paulgg-code/Blockchain-message-esp32.git
   cd Blockchain-message-esp32

   nano .env --> Make sure to set the .env file with QUICKNODE_ENDPOINT  (RPC) and PRIVATE_KEY.
   QUICKNODE_ENDPOINT="XXXX"
   PRIVATE_KEY="XXXX"
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. Update the configuration:
   - Replace the `contractAddress` in `index.js` with your deployed contract address.

4. Run the backend server:
   ```bash
   node index.js
   ```

### ESP32 Firmware Setup

1. Connect the ESP32 device to your computer.
2. Install the required libraries in the Arduino IDE or PlatformIO:
   - `WiFi.h`
   - `HTTPClient.h`
   - `mbedtls` library for ECC key generation and encryption.
3. Configure the Wi-Fi credentials in the ESP32 code:
   ```cpp
   WiFi.begin("your-SSID", "your-password");
   ```
4. Set the backend server IP in the ESP32 code:
   ```cpp
   #define SERVER_URL "http://<backend-ip>:3000/"
   ```
5. Upload the code to your ESP32 device.

## Usage

1. **Start the Backend:**
   Run the backend on your server or local machine:
   ```bash
   node index.js
   ```

2. **Register the ESP32 Device:**
   When the ESP32 device boots up, it will connect to Wi-Fi, generate an ECC key pair, and send a registration request to the backend. You should see the device registration details in the backend logs.

3. **List Registered Devices:**
   You can use the `/get_registered_devices` endpoint to list all devices registered on the blockchain.

4. **Send Encrypted Messages:**
   After registering, the ESP32 device can select a recipient device from the registered devices and send encrypted messages via the backend and blockchain.

## API Endpoints

The backend exposes several REST API endpoints:

- `POST /register`: Registers a new device by storing its public key on the blockchain.
- `GET /get_registered_devices`: Lists all devices registered on the blockchain.
- `POST /send_message`: Sends an encrypted message between devices.
- `GET /get_messages`: Retrieves all messages for a given device.

### Example API Call

To register a new device, the ESP32 sends a POST request to `/register`:
```json
{
  "device_id": "esp32_device_1",
  "public_key": "<base64-encoded-public-key>"
}
```

## Notes

- Ensure the blockchain account used has enough balance to perform transactions.
- The backend uses the ethers.js 6.x library, which has API changes compared to older versions.
- The smart contract must be deployed and reachable via the provided RPC URL in the backend.

## Troubleshooting

- **ESP32 Connection Issues:** Make sure the Wi-Fi credentials are correct and the backend server is reachable from the ESP32 device.
- **Smart Contract Errors:** Ensure that the contract is correctly deployed on the blockchain and the ABI is correct in the backend.

## License

This project is licensed under the MIT License.
