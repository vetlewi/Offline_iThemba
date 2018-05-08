

#include <stdio.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <deque>
#include <algorithm>

#include <iostream>

#define SPLITFILES 0

typedef struct {
	int64_t timestamp;
    uint16_t address;
	uint32_t raw_data[12];
	int raw_size;
} Event_t;

inline int mod_num(int address){

	return (address - 32)/16;
}

int time_const[7] = {10, 10, 8, 8, 8, 8, 8};

bool ExtractNext(FILE *file, Event_t &event)
{
	// First check if EOF has been reached.
	uint32_t h1;
	if (fread(&h1, sizeof(uint32_t), 1, file) != 1) // EOF or error...
		return false;

	// Get size of event
	uint16_t event_length = ( h1 & 0x3FFE0000) >> 17;
    uint16_t header_length = ( h1 & 0x0001F000) >> 12;
    uint16_t address = ( h1 & 0x00000FFF );
    event.address = address;

    // If event_length or header_length == 0, then return false
    if (event_length == 0 || header_length == 0)
    	return false;

    // Raw data to store the extracted data belonging to the event
    uint32_t *event_data = new uint32_t[event_length];

    // Read the entire event from file
    if (fread(event_data+1, sizeof(uint32_t), event_length-1, file) != event_length - 1)
    	return false;
    event_data[0] = h1;
    // Parse the timestamp
    int64_t tmp1 = event_data[1];
    int64_t tmp2 = (event_data[2] & 0x0000FFFF);
    event.timestamp = tmp2 << 32;
    event.timestamp |= tmp1;

    // We will remove any traces
    if (event_length != header_length){
    	uint32_t new_f_head = header_length << 17;
    	new_f_head |= (event_data[0] & 0x8001FFFF);
    	event_data[0] = new_f_head;
    	event_data[3] = (event_data[3] & 0x0000FFFF);
    }

    // Copy raw event to the event structure.
    for (int i = 0 ; i < header_length ; ++i)
    	event.raw_data[i] = event_data[i];
    event.raw_size = header_length;
    
    event.timestamp *= time_const[mod_num(address)];

	// Delete temporary memory
	delete[] event_data;
    return true; // Done!

}

std::string OutputFileName(std::string filename, int i)
{
    std::string base_name = filename.substr(0, filename.find(".data"));
    base_name += '_';
    base_name += ('A' + i);
    base_name += ".data"; 
    return base_name;
}

void SplitFile(std::string filename)
{
    FILE *input = fopen(filename.c_str(), "rb");
    FILE *output = fopen(OutputFileName(filename, 0).c_str(), "wb");
    int files = 0;
    Event_t last_event, now_event;
    int64_t n_ok=0, n_err=0, n_tot=0;
    if ( !ExtractNext(input, last_event) ){
        fclose(input);
        fclose(output);
        return;
    }

    fwrite(last_event.raw_data, sizeof(uint32_t), last_event.raw_size, output);

    while (ExtractNext(input, now_event)){

        if (now_event.timestamp >= last_event.timestamp){
            ++n_ok;
            fwrite(now_event.raw_data, sizeof(uint32_t), now_event.raw_size, output);
        }
        else {
            ++n_err;
            fclose(output);
            output = fopen(OutputFileName(filename, ++files).c_str(), "wb");
            fwrite(now_event.raw_data, sizeof(uint32_t), now_event.raw_size, output);
        }
        ++n_tot;
        last_event = now_event;
    }

    fclose(input);
    fclose(output);


    std::cout << "Results of file '" << filename << "':" << std::endl;
    std::cout << "Total number of events: " << n_tot << std::endl;
    std::cout << "Total number of ordered words: " << n_ok << std::endl;
    std::cout << "Total number of unordered words: " << n_err << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}

void CheckFile(std::string filename)
{
    FILE *input = fopen(filename.c_str(), "rb");

    Event_t last_event, now_event;
    int64_t n_ok=0, n_err=0, n_tot=0;
    
    std::cout << "Results of file '" << filename << "':" << std::endl;    

    if ( !ExtractNext(input, last_event) ){
        fclose(input);
        return;
    }


    while (ExtractNext(input, now_event)){

        if (now_event.timestamp >= last_event.timestamp){
            ++n_ok;
        }
        else {
            ++n_err;
            std::cout << last_event.timestamp << " " << now_event.timestamp << std::endl;
        }
        ++n_tot;
        last_event = now_event;
    }

    fclose(input);


    std::cout << "Total number of events: " << n_tot << std::endl;
    std::cout << "Total number of ordered words: " << n_ok << std::endl;
    std::cout << "Total number of unordered words: " << n_err << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
}

int main(int argc, char *argv[])
{


    std::vector<std::string> input_files;

    if (argc <= 1){
        std::cout << "Usage: >" << argv[0] << "<path/to/file1> <path/to/file2> ..." << std::endl;
    } else {
        for (int i = 1 ; i < argc ; ++i){
            input_files.push_back(argv[i]);
        }
    }

	for (size_t i = 0 ; i < input_files.size() ; ++i){
        #if SPLITFILES
            SplitFile(input_files[i]);
        #else
            CheckFile(input_files[i]);
        #endif // SPLITFILES
    }

	return 0;
}
