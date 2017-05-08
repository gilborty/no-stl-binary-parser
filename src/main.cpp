#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include "Battery.h"

// Keys
constexpr uint8_t newLineChar = 0x0a;
constexpr uint8_t commentKey = 0x3b; // ;
constexpr uint8_t compareKey = 0x43; // C
constexpr uint8_t spaceKey = 0x20;
constexpr uint8_t waitKey = 0x58; // X
constexpr uint8_t writeKey = 0x57; // W

// "Each line of the file represents one command and potentially 96 bytes of data..."
// Reference: http://www.ti.com/lit/an/slua541a/slua541a.pdf
constexpr int MAX_DATA_BYTES_PER_LINE = 1024;

// Is it a new line?
bool isNewLine = true;

// Helpers
int handleWait(const uint8_t* buffer, size_t length)
{
    // Wait command structure:
    // X: [wait in ms]

    // Iterate through the payload
    // First three chars (W,:, ,) are diregarded
    // TODO: Is this safe to assume (speed vs safety)
    
    size_t payloadLength = length - 3;
    char payload[payloadLength];

    for(size_t i = 3; i < length; ++i)
    {
        // Pack the payload
        auto payloadIndex = i - 3;
        payload[payloadIndex] = buffer[i];
    }

    // Convert to an integer wait in ms
    int waitTime;
    waitTime = static_cast<int>(atoi(payload));
    std::cout << "Waiting for " << waitTime << " ms" << std::endl;

    return 0;
}

int main(int argc, const char* argv[])
{
    std::cout << "Length of array: " << (sizeof(k_battery)/sizeof(*k_battery)) << std::endl;

    size_t lengthOfBatteryData = (sizeof(k_battery)/sizeof(*k_battery));

    // Buffer for reading data lines
    uint8_t buffer[MAX_DATA_BYTES_PER_LINE]; 

    int counter = 0;
    int lineCounter = 0;

    uint8_t key = 0x00;

    // iterate through the firmware
    for(size_t i = 0; i < lengthOfBatteryData; ++i)
    {
        // Check the key
        if(isNewLine)
        {
            std::cout << "Got a new line." << std::endl;
            std::cout << "Key: " << k_battery[i] << std::endl;
            key = k_battery[i];

            isNewLine = false;
        }

        // Add data to the data array until we get a new line
        buffer[counter] = k_battery[i];
        ++counter;

        if(k_battery[i] == newLineChar)
        {
            // Finished a line
            ++lineCounter;
            isNewLine = true;

            // Switch on the value
            switch(key)
            {
                case(commentKey):
                {
                    std::cout << "Got a comment." << std::endl;
                    break;
                }
                case(compareKey):
                {
                    std::cout << "Got a compareKey" << std::endl;
                    break;
                }
                case(waitKey):
                {
                    std::cout << "Got a wait" << std::endl;
                    auto ret = handleWait(buffer, counter);
                    std::cout << "Ret code: " << ret << std::endl;
                    break;
                }
                case(writeKey):
                {
                    std::cout << "Got a write" << std::endl;
                    break;
                }
                default:
                {
                    std::cout << "Unrecongnized key" << std::endl;
                    break;
                }
            }


            std::cout << std::endl;

            // Reset
            counter = 0;
            memset(buffer, 0, MAX_DATA_BYTES_PER_LINE);
        }
    }
    return 0;
}
