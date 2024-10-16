require("dotenv").config();
const express = require('express');
const bodyParser = require('body-parser');
const { ethers } = require('ethers');

const app = express();
app.use(bodyParser.json());

// Replace with your actual contract address and ABI
const contractAddress = "0x98Dff9baD6fD1D61F21386425b92df964210FcbB";
const abi = [
	{
		"inputs": [
			{
				"internalType": "string",
				"name": "",
				"type": "string"
			}
		],
		"name": "devices",
		"outputs": [
			{
				"internalType": "string",
				"name": "deviceId",
				"type": "string"
			},
			{
				"internalType": "bool",
				"name": "isRegistered",
				"type": "bool"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [],
		"name": "getAllRegisteredDevices",
		"outputs": [
			{
				"internalType": "string[]",
				"name": "",
				"type": "string[]"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "string",
				"name": "_recipient",
				"type": "string"
			}
		],
		"name": "getMessagesForRecipient",
		"outputs": [
			{
				"internalType": "string[]",
				"name": "",
				"type": "string[]"
			},
			{
				"internalType": "string[]",
				"name": "",
				"type": "string[]"
			},
			{
				"internalType": "string[]",
				"name": "",
				"type": "string[]"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "uint256",
				"name": "",
				"type": "uint256"
			}
		],
		"name": "messages",
		"outputs": [
			{
				"internalType": "string",
				"name": "sender",
				"type": "string"
			},
			{
				"internalType": "string",
				"name": "recipient",
				"type": "string"
			},
			{
				"internalType": "string",
				"name": "message",
				"type": "string"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "string",
				"name": "_deviceId",
				"type": "string"
			}
		],
		"name": "registerDevice",
		"outputs": [],
		"stateMutability": "nonpayable",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "uint256",
				"name": "",
				"type": "uint256"
			}
		],
		"name": "registeredDevices",
		"outputs": [
			{
				"internalType": "string",
				"name": "",
				"type": "string"
			}
		],
		"stateMutability": "view",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "string",
				"name": "_deviceId",
				"type": "string"
			}
		],
		"name": "removeDevice",
		"outputs": [],
		"stateMutability": "nonpayable",
		"type": "function"
	},
	{
		"inputs": [
			{
				"internalType": "string",
				"name": "_sender",
				"type": "string"
			},
			{
				"internalType": "string",
				"name": "_recipient",
				"type": "string"
			},
			{
				"internalType": "string",
				"name": "_message",
				"type": "string"
			}
		],
		"name": "storeMessage",
		"outputs": [],
		"stateMutability": "nonpayable",
		"type": "function"
	}
];

// Configure the blockchain provider and wallet
const provider = new ethers.JsonRpcProvider(process.env.QUICKNODE_ENDPOINT);
const wallet = new ethers.Wallet(process.env.PRIVATE_KEY, provider);
const contract = new ethers.Contract(contractAddress, abi, wallet);

// Endpoint to register a new device
app.post('/register', async (req, res) => {
    const { device_id } = req.body;
    console.log(`Registering device: ${device_id}`);

    if (!device_id) {
        return res.status(400).json({ error: "Device ID is required" });
    }

    try {
        const tx = await contract.registerDevice(device_id);
        console.log(`Transaction submitted. Tx hash: ${tx.hash}`); // Log the transaction hash
        await tx.wait(); // Wait for the transaction to be mined
        console.log(`Transaction confirmed. Device ${device_id} registered successfully.`);

        res.status(200).json({ status: "Device registered successfully", txHash: tx.hash });
    } catch (error) {
        console.error("Error registering device:", error);
        res.status(500).json({ error: "Failed to register device" });
    }
});

// Endpoint to send a message
app.post('/send_message', async (req, res) => {
    const { sender, recipient, message } = req.body;
    console.log(`Storing message from ${sender} to ${recipient}: ${message}`);

    if (!sender || !recipient || !message) {
        return res.status(400).json({ error: "Missing required fields: sender, recipient, or message" });
    }

    try {
        const tx = await contract.storeMessage(sender, recipient, message);
        console.log(`Transaction submitted. Tx hash: ${tx.hash}`); // Log the transaction hash
        await tx.wait();
        console.log(`Transaction confirmed. Message stored on blockchain.`);

        res.status(200).json({ status: "Message stored on blockchain", txHash: tx.hash });
    } catch (error) {
        console.error("Error storing message:", error);
        res.status(500).json({ error: "Failed to store message" });
    }
});

// Endpoint to get registered devices
app.get('/get_registered_devices', async (req, res) => {
    try {
        const devices = await contract.getAllRegisteredDevices();
        res.status(200).json(devices);
    } catch (error) {
        console.error("Error retrieving registered devices:", error);
        res.status(500).json({ error: "Failed to get registered devices" });
    }
});

// Endpoint to get messages for a recipient
app.get('/get_messages', async (req, res) => {
    const { recipient } = req.query;

    if (!recipient) {
        return res.status(400).json({ error: "Recipient ID is required" });
    }

    try {
        const [senders, recipients, messages] = await contract.getMessagesForRecipient(recipient);
        const result = senders.map((sender, index) => ({
            sender: sender,
            recipient: recipients[index],
            message: messages[index]
        }));
        res.status(200).json(result);
    } catch (error) {
        console.error("Error getting messages:", error);
        res.status(500).json({ error: "Failed to get messages" });
    }
});

// Error handler for JSON parsing errors
app.use((err, req, res, next) => {
    if (err instanceof SyntaxError && err.status === 400 && 'body' in err) {
        console.error('Bad JSON:', req.body);
        return res.status(400).json({ error: "Invalid JSON" });
    }
    next();
});

// Start the server
const port = process.env.PORT || 3000;
app.listen(port, () => {
    console.log(`Server running on port ${port}`);
});