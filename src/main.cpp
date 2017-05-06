#include <iostream>
#include <cstring>

#include "Battery.h"

int main(int argc, const char* argv[])
{
    std::cout << "Length of array: " << (sizeof(k_battery)/sizeof(*k_battery)) << std::endl;

    size_t lengthOfBatteryData = (sizeof(k_battery)/sizeof(*k_battery));

    constexpr uint8_t newLineChar = 0x0a;

    // "Each line of the file represents one command and potentially 96 bytes of data..."
    // Reference: http://www.ti.com/lit/an/slua541a/slua541a.pdf
    constexpr int MAX_DATA_BYTES_PER_LINE = 1024;
    uint8_t buffer[MAX_DATA_BYTES_PER_LINE]; 

    int counter = 0;
    int lineCounter = 0;

    for(size_t i = 0; i < lengthOfBatteryData; ++i)
    {
        // Add data to the array 
        buffer[counter] = k_battery[i];
        ++counter;

        if(k_battery[i] == newLineChar)
        {
            ++lineCounter;
            for(size_t k = 0; k < counter; ++k)
            {
                std::cout << (buffer[k]) << ", ";
            }
            std::cout << std::endl;

            // Reset
            counter = 0;
            memset(buffer, 0, MAX_DATA_BYTES_PER_LINE);
        }
    }
    return 0;
}
