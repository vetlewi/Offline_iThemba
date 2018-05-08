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

#ifndef OFFLINESORTING_H
#define OFFLINESORTING_H

#include <string>
#include <memory>
#include <atomic>

#include "RateMeter.h"
#include "Event.h"

//#define MTSORTING

class FileBufferFetcher;
class Unpacker;
class UserRoutine;
class WordBuffer;


struct FormatStr {
    FileBufferFetcher *bf;
    Unpacker *up;
};


/*!
 *  \class     OfflineSorting
 *  \brief     A class for handling offline sorting.
 *  \details   Class reading and executing commands from input batch file.
 *  \author    Vetle W. Ingeberg
 *  \version   0.9.0
 *  \date      2015-2017
 *  \copyright GNU Public License v. 3
 */
class OfflineSorting {
public:

    //! Initialize sorting session.
    /*! By default, no maximum buffer number is set and the files are
     *  read using MTFileBufferFetcher.
     */
    OfflineSorting(UserRoutine& us	/*!< The sorting routine to use. */);

    //! Initilize sorting session.
    /*! Passing with a structure containg pointers to all objects
     *  that should be used.
     */
    OfflineSorting(UserRoutine& us,		/*!< Sorting routine		*/
                   FormatStr *fs		/*!< Buffer fetcher type	*/);

    //! Destructor.
    ~OfflineSorting();

    //! Run all the commands in the batch file.
    void Run(const std::string &batchfilename	/*!< Filename of the batch file to read.	*/);

    // Convinient helper for a short main() routine.
    /*! Can be use as following:
     *  \code
     *  int main(int argc, char* argv[])
     *  {
     *		return OfflineSorting::Run(new UserRoutine, argc, argv);
        }
     * \endcode
     */
    static int Run(UserRoutine* ur, /*!< User implementation of the event sorting routine.  */
                   int argc,        /*!< No. of command line arguments.                     */
                   char* argv[]     /*!< The command line arguments.                        */);

    //! Overload of Run.
    /*! Can be used to give another file buffer reader class than the default. Is used as following:
     *  \code
     *  int main(int argc, char* argv[])
     *  {
     *      return OfflineSorting::Run(new UserRoutine, new FileBufferFetcher, argc, argv);
     *  }
     *  \endcode
     */
    static int Run(UserRoutine *ur,         /*!< User implementation of the event sorting routine.          				*/
                   FormatStr *bf,   		/*!< Structure with pointers to object used for the particular raw data format	*/
                   int argc,                /*!< No. of command line arguments.                             				*/
                   char *argv[]             /*!< The command line arguments.                                				*/);

    //! Set the maximum number of buffers to read per file.
    void SetMaxBuffers(int maxBuffers	/*!< The maximum buffer count.	*/);

    //! Set the object to fetch buffers.
    void SetBufferFetcher(FileBufferFetcher *bf	/*!< The object used to read the data files.	*/);

    //! Sort one file.
    /*! \return true if everything is okey; else false.
     */
    bool SortFile(const std::string filename,   /*!< The name of the file to read.  */
                  int begin,              		/*!< Where to begin.                */
                  int end                		/*!< Where to end.                  */);

protected:
    //! Sort one buffer.
    /*! \return true if everything is okey; else false.
     */
    bool SortBuffer(const WordBuffer* buffer /*!< The buffer to sort. */);

private:
    //! Variable to contain user routine.
    UserRoutine& userSort;

    //! Write to command line.
    bool is_tty;

    //! Maximum number of buffers per file.
    int maxBuffers;

    //! Filereader object.
    std::unique_ptr<FileBufferFetcher> bufferFetcher;

    //! Object performing unpacking of the data.
    std::unique_ptr<Unpacker> unpacker;

    //! Ratemeter for buffer rate messurements.
    RateMeter rateMeter;

    //! The prefix for paths given in "data file" commands.
    std::string data_directory;

    //! Number of events unpacked.
    int nEvents;

    //! Handles 'export' commands.
    /*!
     *  \return true if everything is okey; else false.
     */
    bool export_command(std::istream& icmd);

    //! Handles 'data' commands.
    /*! Reads the parameters and performs sorting of the file.
     *  \return true if everything is okey; else false.
     */
    bool data_command(std::istream& icmd);

    //! Handles a command.
    /*! \return true if everything is okey; else false.
     */
    bool next_command(const std::string& cmd /*!< The command. */);

    //! Get next command from the input stream.
    /*! Input will be the batch file.
     *  \return true if successful.
     */
    bool next_commandline(std::istream& in, std::string& cmd_line);


};

#endif // OFFLINESORTING_H
