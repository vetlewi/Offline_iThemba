/*******************************************************************************
 * Copyright (C) 2016 Vetle W. Ingeberg                                        *
 * Author: Vetle Wegner Ingeberg, v.w.ingeberg@fys.uio.no                      *
 *                                                                             *
 * --------------------------------------------------------------------------- *
 * This program is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the       *
 * Free Software Foundation; either version 3 of the license, or (at your      *
 * option) any later version.                                                  *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but         *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General   *
 * Public License for more details.                                            *
 *                                                                             *
 * You should have recived a copy of the GNU General Public License along with *
 * the program. If not, see <http://www.gnu.org/licenses/>.                    *
 *                                                                             *
 *******************************************************************************/

/*!
 * \file OfflineSorting.cpp
 * \brief Implementation of the OfflineSorting class.
 * \author Vetle W. Ingeberg
 * \version 0.8.0
 * \date 2015-2016
 * \copyright GNU Public License v. 3
 */

#include "OfflineSorting.h"

#include "UserRoutine.h"

#include "FileBufferFetcher.h"
#include "STFileBufferFetcher.h"
#include "MTFileBufferFetcher.h"

#include "WordBuffer.h"
#include "Event.h"

#include "Unpacker.h"

#include "RootWriter.h"
#include "MamaWriter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <unistd.h>


//! Global variable signaling if the sorting has been interrupted.
static char leaveprog = 'n';

//! Signal handler for Ctrl-C.
static void keyb_int(int sig_num)
{
    if (sig_num == SIGINT) {
        printf("\n\nProgram interrupted by user...\n");
        leaveprog = 'y';
    }
}

//! Remove witespace at beginning and end.
/*! \return The text without leading and trailing whitespace.
 */
static std::string trim_whitespace( const std::string& text /*!< Text to be clean. */)
{
    size_t start = text.find_first_not_of(" \t");
    if ( start == std::string::npos )
        return "";

    size_t end = text.find_last_not_of(" \t");
    if ( end == std::string::npos || end<=start )
        end = text.size();
    return text.substr(start, end-start+1);
}

// ########################################################################
// ########################################################################
// ########################################################################

OfflineSorting::OfflineSorting(UserRoutine& us)
    : userSort( us )
    , is_tty( isatty(STDOUT_FILENO) )
    , maxBuffers( -1 )
    , bufferFetcher( new MTFileBufferFetcher )
    , unpacker( new Unpacker )
    , rateMeter( 500, !is_tty )
    {
        signal(SIGINT, keyb_int); // Setting up interrupt handler (Ctrl-C)
        signal(SIGPIPE, SIG_IGN);
    }

// ########################################################################

OfflineSorting::OfflineSorting(UserRoutine &us, FormatStr *fs)
    : userSort( us )
    , is_tty( isatty(STDOUT_FILENO) )
    , maxBuffers( -1 )
    , bufferFetcher( fs->bf )
    , unpacker( fs->up )
    , rateMeter( 500, !is_tty )
{
    signal(SIGINT, keyb_int); // Setting up interrupt handler (Ctrl-C)
    signal(SIGPIPE, SIG_IGN);
}

// ########################################################################

OfflineSorting::~OfflineSorting()
{

}

// ########################################################################

void OfflineSorting::SetBufferFetcher(FileBufferFetcher *bf)
{
    bufferFetcher.reset(bf);
}

// ########################################################################

void OfflineSorting::SetMaxBuffers(int mb)
{
    maxBuffers = mb;
}

// ########################################################################

bool OfflineSorting::SortBuffer(const WordBuffer* buffer) // This will run in the main Thread
{
    Event event;
    unpacker->SetBuffer(buffer);
    Unpacker::Status ustat = Unpacker::END;
    // We start the sorter thread...
    // Command start thread.


    while (leaveprog == 'n'){
        ustat = unpacker->Next(event);
        if ( ustat != Unpacker::OKAY )
            break;
        userSort.Sort(event);
        nEvents += 1;
    }
    return ustat == Unpacker::END;
}

// ########################################################################

bool OfflineSorting::SortFile(const std::string filename, int buf_start, int buf_end)
{
    // Open data file.
    if (bufferFetcher->Open(filename, buf_start) != BufferFetcher::OKAY){
        std::cerr << "Data: Could not open '" << filename << "'" << std::endl;
        return false;
    }

    int buffer_count = 0, bad_buffer_count = 0;
    rateMeter.Reset();
    //unpacker->Reset();

    // Fetch next buffer.
    BufferFetcher::Status fstate;
    float bufs_per_sec;
    nEvents = 0;

    // Loop over all buffers
    for (int b = buf_start ; buf_end < 0 || b < buf_end ; ++b){
        // Stop if Ctrl-C has been passed
        if (leaveprog != 'n'){
            break;
        }

        const WordBuffer* buf = bufferFetcher->Next(fstate);
        if ( fstate == BufferFetcher::END ){
            break;
        } else if ( fstate == BufferFetcher::ERROR) {
            std::cerr << "\ndata: error reading buffer " << b << " in file '" << filename << "'" << std::endl;
            return false;
        }

        // Sort buffer
        buffer_count += 1;
        bool sort_ok = SortBuffer(buf);
        if ( !sort_ok )
            bad_buffer_count += 1;


        bufs_per_sec = rateMeter.Rate();
        if ( bufs_per_sec > 0 ){
            if ( is_tty ){
                std::cout << "      " << std::flush << '\r' // Clear the line
                          << buffer_count << '/' << bad_buffer_count
                          << ' ' << unpacker->GetAverageLength() << " hits/event"
                          << ' ' << double(nEvents)/buffer_count << " event/bufs"
                          << ' ' << bufs_per_sec*buf->GetSize() << " hits/s " << std::flush;
            } else {
                std::cout << '.' << std::flush;
            }
        }
    }

    // Print counter and rate at the end
    std::cout << '\r' << buffer_count << '/' << bad_buffer_count
              << ' ' << unpacker->GetAverageLength() << " hits/event"
              << ' ' << double(nEvents)/buffer_count << " event/bufs"
              << ' ' << rateMeter.TotalRate()*WordBuffer::BUFSIZE
              << " hits/s " << std::endl;
    return true;

}

// ########################################################################

bool OfflineSorting::data_command(std::istream& icmd)
{
    int buf_start=0, buf_end=maxBuffers;

    std::string tmp;
    icmd >> tmp;
    if ( tmp == "directory" ){
        icmd >> tmp;
        std::string dd = trim_whitespace( tmp );
        if ( dd.empty() ){
            std::cerr << "data: Do not understand directory '" << tmp << "'" << std::endl;
            return false;
        }
        data_directory = dd;
        std::cout << "Set data directory to '" << data_directory << "'" << std::endl;
        return true;
    }

    if ( tmp == "buffers" ){
        icmd >> buf_start >> buf_end >> tmp;
        if ( maxBuffers > 0 )
            buf_end = std::min(buf_end, buf_start+maxBuffers);
    }

    if ( tmp != "file" ){
        std::cerr << "data: Expected 'data [buffers <from> <to>] file <filename>'\n";
        return false;
    }

    // Extract filename, trying to allow filenames with spaces inside.
    std::string filnam;
    std::getline( icmd, filnam );
    std::string filename = trim_whitespace( filnam );
    if ( filename.empty() ){
        std::cerr << "data: Do not understand filename '" << filename << "'" << std::endl;
        return false;
    }

    // Prepend data_directory, but only for relative paths
    if ( !data_directory.empty() && filename[0] != '/')
        filename = data_directory + "/" + filename;

    // Write to screen that we will read file.
    std::cout << "data: Reading file '" << filename
              << "' buffers [" << buf_start << ',';
    if (buf_end < 0)
        std::cout << "end";
    else
        std::cout << buf_end;
    std::cout << "]" << std::endl;

    return SortFile(filename, buf_start, buf_end);
}

// ########################################################################

bool OfflineSorting::export_command(std::istream& icmd)
{
    std::string tmp;
    icmd >> tmp;
    if (tmp == "root"){
        icmd >> tmp;
        std::string rootfile = trim_whitespace( tmp );
        if (rootfile.empty()){
            std::cerr << "Export root: Do not understand ROOT filename: '" << tmp << "'" << std::endl;
            return false;
        }
        RootWriter::Write( userSort.GetHistograms(), rootfile );
        std::cout << "export as ROOT file into '" << tmp << "'" << std::endl;
        std::cout << "Resetting all histograms" << std::endl;
        userSort.GetHistograms().ResetAll();
        return true;
    } else if (tmp == "mama"){
        icmd >> tmp;
        std::string histname = trim_whitespace(tmp);
        if ( histname.empty() ){
            std::cerr << "export mama: Do not understand histogram name '" << histname << "'" << std::endl;
            return false;
        }
        Histogram1Dp h = userSort.GetHistograms().Find1D(histname);
        Histogram2Dp m = userSort.GetHistograms().Find2D(histname);
        if (!h && !m){
            std::cerr << "export mama: No histogram named '"
                      << histname << "'" << std::endl;
            return false;
        }

        icmd >> tmp;
        std::string mamafile = trim_whitespace(tmp);
        if (mamafile.empty()){
            std::cerr << "export mama: Do not understand mama filename '"
                      << mamafile << "'" << std::endl;
            return false;
        }
        std::ofstream mama_out( mamafile.c_str() );
        if ( !mama_out ){
            std::cerr << "export mama: problem opening '"
                      << mamafile << "'" << std::endl;
            return false;
        }

        std::cout << "Export '" << histname << "' as MAMA file into '"
                  << mamafile << "'" << std::endl;
        if ( h )
            MamaWriter::Write(mama_out, h);
        else if ( m )
            MamaWriter::Write(mama_out, m);
        if ( !mama_out ){
            std::cerr << "export mama: Problem writing '"
                      << mamafile << "'." << std::endl;
            return false;
        }
        mama_out.close();
        return true;
    }
    return false;
}

// ########################################################################

bool OfflineSorting::next_command(const std::string& cmd)
{
    std::istringstream icmd(cmd.c_str());

    std::string name, tmp;
    icmd >> name;

    if (name == "quit"){
        leaveprog = 'y';
        return true;
    } else if ( name == "data" ){
        return data_command(icmd);
    } else if ( name == "export" ){
        return export_command(icmd);
    } else if ( name == "reset_histograms"){
        userSort.GetHistograms().ResetAll();
        return true;
    } else {
        return userSort.Command(cmd);
    }
}

bool OfflineSorting::next_commandline(std::istream& in, std::string& cmd_line)
{
    cmd_line = "";
    std::string line;
    while ( getline(in, line) ){
        size_t ls = line.size();
        if ( ls == 0 ){
            break;
        } else if ( line[ls-1] != '\\'){
            cmd_line += line;
            break;
        } else {
            cmd_line += line.substr(0, ls-1);
        }
    }
    return in || !cmd_line.empty();
}

void OfflineSorting::Run(const std::string& batchfilename)
{
    std::ifstream batch_file(batchfilename.c_str());
    std::string batch_line;
    while ( leaveprog == 'n' && next_commandline(batch_file, batch_line) ){
        if ( batch_line.size() == 0 || batch_line[0] == '#' )
            continue;
        if ( !next_command(batch_line) ) {
            std::cerr << "Do not understand batch line: '" << batch_line << "'" << std::endl;
            break;
        }
    }
}

int OfflineSorting::Run(UserRoutine* ur, int argc, char* argv[])
{
    if (argc != 2){
        std::cerr << "Run like: " << argv[0] << " <batchfile>" << std::endl;
        return -1;
    }

    ur->Start();
    OfflineSorting offline( *ur );
    offline.Run( argv[1] );
    ur->End();
    delete ur;
    return 0;
}

int OfflineSorting::Run(UserRoutine* ur, FormatStr* fs, int argc, char* argv[])
{
    if (argc != 2){
        std::cerr << "Run like: " << argv[0] << " <batchfile>" << std::endl;
        return -1;
    }

    ur->Start();
    OfflineSorting offline(*ur, fs);
    offline.Run( argv[1] );
    ur->End();
    delete ur;
    return 0;
}
