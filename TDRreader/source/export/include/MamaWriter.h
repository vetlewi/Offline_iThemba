// -*- c++ -*-

#ifndef MamaWriter_H_
#define MamaWriter_H_ 1

#include <iosfwd>
#include <memory>

class Histogram1D;
class Histogram2D;

typedef std::shared_ptr<Histogram1D> Histogram1Dp;
typedef std::shared_ptr<Histogram2D> Histogram2Dp;

/*!
 * \class MamaWriter
 * \brief Class for writing Histograms in MAMA format.
 * \details This class writes MAMA formatted histograms to disk.
 * \author unknown
 * \copyright Copyfree?
 */
class MamaWriter {
public:

    //! Write a single 1D histogram in MAMA format.
    /*! \return 0 if okay, <0 if error
     */
    static int Write(std::ofstream& out, /*!< The output stream to write to. */
                     Histogram1Dp h      /*!< The histogram to write. */);

    //! Write a single 2D histogram in MAMA format.
    /*! \return 0 if okay, <0 if error
     */
    static int Write(std::ofstream& out, /*!< The output stream to write to. */
                     Histogram2Dp h      /*!< The histogram to write. */);

};

#endif /* MamaWriter_H_ */
