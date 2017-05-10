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
    // Token is first three chars [X: ]

    // Allocate a data string
    char dataString[length+1];
    memcpy(dataString, buffer, length+1);
    
    // Add new line to buffer so strtok works
    dataString[length+1] = '\n';

    // Get the wait time
    char* pch;
    int waitTime = 0;
    pch = strtok(dataString,"X: ");

    while(pch != NULL)
    {
        waitTime = static_cast<int>(atoi(pch));
        pch = strtok(NULL, " ");
    }

    std::cout << "Wait time: " << waitTime <<  std::endl;
    if(waitTime == 0)
    {
        // Bad string, report error
        return -1;
    }

    // TODO: Logic to wait for x ms using hasElapsed();
    return 0;
}

int handleWrite(const uint8_t* buffer, size_t length)
{
    // Write command structure:
    // W: [i2caddr][regaddr] [payload]

    // First three chars (W,:, ,) are diregarded
    char dataString[length+1];
    memcpy(dataString, buffer, length+1);

    // Add new line to buffer so strtok works
    dataString[length+1] = '\n';

    // A placeholder to walk the pointer through the data string
    char* tokenPointer;

    // W: 
    char* key;

    // The i2c address
    char* i2cAddress;

    // The register address
    char* regAddress;

    key = strtok_r(dataString, " ", &tokenPointer);
    i2cAddress = strtok_r(NULL, " ", &tokenPointer);
    regAddress = strtok_r(NULL, " ", &tokenPointer);

    std::cout << "Writing to i2c: " << i2cAddress << " at reg: " << regAddress << " with data: " << tokenPointer << std::endl;

    // TODO: Write over i2c

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
            //std::cout << "Got a new line." << std::endl;
            //std::cout << "Key: " << k_battery[i] << std::endl;
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
                    //std::cout << "Got a comment. No action." << std::endl;
                    break;
                }
                case(compareKey):
                {
                    //std::cout << "Handling compare: " << std::endl;
                    break;
                }
                case(waitKey):
                {
                    std::cout << "Handling wait: " << std::endl;
                    auto ret = handleWait(buffer, counter);
                    std::cout << "Ret code: " << ret << std::endl;
                    break;
                }
                case(writeKey):
                {
                    std::cout << "Handling Write: " << std::endl;
                    auto ret = handleWrite(buffer, counter);
                    std::cout << "Ret Code: " << ret << std::endl;
                    break;
                }
                default:
                {
                    std::cout << "Unrecongnized key. No action" << std::endl;
                    break;
                }
            }

            // Pretty pretty
            //std::cout << std::endl;

            // Reset
            counter = 0;
            memset(buffer, 0, MAX_DATA_BYTES_PER_LINE);
        }
    }
    return 0;
}
