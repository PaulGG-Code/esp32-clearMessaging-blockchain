// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

contract DeviceRegistry {
    struct Device {
        string deviceId;
        bool isRegistered;
    }

    struct Message {
        string sender;
        string recipient;
        string message;
    }

    mapping(string => Device) public devices;
    Message[] public messages;
    string[] public registeredDevices;

    // Register a device
    function registerDevice(string memory _deviceId) public {
        require(!devices[_deviceId].isRegistered, "Device is already registered");

        devices[_deviceId] = Device({
            deviceId: _deviceId,
            isRegistered: true
        });

        registeredDevices.push(_deviceId); // Add device to the list of registered devices
    }

    // Retrieve all registered devices
    function getAllRegisteredDevices() public view returns (string[] memory) {
        return registeredDevices;
    }

    // Store a plain text message on the blockchain
    function storeMessage(string memory _sender, string memory _recipient, string memory _message) public {
        messages.push(Message({
            sender: _sender,
            recipient: _recipient,
            message: _message
        }));
    }

    // Retrieve messages for a specific recipient
    function getMessagesForRecipient(string memory _recipient) public view returns (string[] memory, string[] memory, string[] memory) {
        uint256 messageCount = 0;

        // Count how many messages are for the recipient
        for (uint256 i = 0; i < messages.length; i++) {
            if (keccak256(abi.encodePacked(messages[i].recipient)) == keccak256(abi.encodePacked(_recipient))) {
                messageCount++;
            }
        }

        // Initialize arrays to hold the data
        string[] memory senders = new string[](messageCount);
        string[] memory recipients = new string[](messageCount);
        string[] memory messageContents = new string[](messageCount);

        uint256 index = 0;

        // Populate the arrays with the matching messages
        for (uint256 j = 0; j < messages.length; j++) {
            if (keccak256(abi.encodePacked(messages[j].recipient)) == keccak256(abi.encodePacked(_recipient))) {
                senders[index] = messages[j].sender;
                recipients[index] = messages[j].recipient;
                messageContents[index] = messages[j].message;
                index++;
            }
        }

        return (senders, recipients, messageContents);
    }

    // Remove a device and all its associated messages
    function removeDevice(string memory _deviceId) public {
        require(devices[_deviceId].isRegistered, "Device is not registered");

        // Remove the device from the devices mapping
        delete devices[_deviceId];

        // Remove the device from the registeredDevices array
        for (uint256 k = 0; k < registeredDevices.length; k++) {
            if (keccak256(abi.encodePacked(registeredDevices[k])) == keccak256(abi.encodePacked(_deviceId))) {
                registeredDevices[k] = registeredDevices[registeredDevices.length - 1];
                registeredDevices.pop();
                break;
            }
        }

        // Remove all messages associated with the device (either as sender or recipient)
        uint256 l = 0;
        while (l < messages.length) {
            if (keccak256(abi.encodePacked(messages[l].sender)) == keccak256(abi.encodePacked(_deviceId)) ||
                keccak256(abi.encodePacked(messages[l].recipient)) == keccak256(abi.encodePacked(_deviceId))) {
                messages[l] = messages[messages.length - 1];
                messages.pop();
            } else {
                l++;
            }
        }
    }
}