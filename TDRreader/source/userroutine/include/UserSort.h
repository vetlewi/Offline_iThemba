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

#ifndef USERSORT_H
#define USERSORT_H

#include "TDRRoutine.h"
#include "Event.h"

typedef struct {
    double energy;  //!< Energy of the event
    int clover_no;  //!< Clover ID.
    int crystal_no; //!< Crystal number (with largest energy deposition)
    word_t max_w;   //!< Word of the hit with the largest energy.
} clover_gamma_t;


class UserSort : public TDRRoutine
{
public:
    UserSort();
    ~UserSort() { }

    bool Sort(const Event& event);

    bool End();

    //! We have no user commands that needs to be set.
    /*! \return true allways.
     */
    bool UserCommand(const std::string &cmd);

protected:
    void CreateSpectra();
    bool SetupTree();

private:

    // Enum describing the 'prompt' status of a gamma-ray
    typedef enum {
        is_prompt,          //!< To indicate that the hit is within the 'prompt' window.
        is_background,      //!< To indicate that the hit is within the 'background' window.
        ignore              //!< To indicate that the hit is neither in the 'prompt' nor the 'background window.
    } prompt_status_t;

    // Method to check if the time is within a gate.
    prompt_status_t CheckTimeStatus(const double &time,         /*!< Time of the hit        */
                                    const Parameter &paramter   /*!< Gates of the detector  */) const;

    // Method for calibrating energy of a detector.
    double CalibrateE(const word_t &w) const;

    // Method for getting time difference between two words.
    double CalcTimediff(const word_t &start, const word_t &stop) const;

    // Method for analyzing and checking conincident gamma events.
    void AnalyzeGamma(const word_t &de_word,    /*!< We need the de_word for the start time         */
                      const double &excitation, /*!< We need the reconstructed excitation energy    */
                      const Event &event,       /*!< Event structure.                               */
                      const clover_gamma_t *cg, /*!< List of gamma rays in the clover detectors     */
                      const int n_clover        /*!< Number of clover events                        */);

    // Method for analyzing and checking coinicident gamma-ppac events.
    void AnalyzeGammaPPAC(const word_t &de_word,    /*!< We need the de_word for the start time         */
                          const double &excitation, /*!< We need the reconstructed excitation energy    */
                          const Event &event        /*!< Event structure.                               */);

    int Addback_Clover(const Event &event, const word_t &align_t, clover_gamma_t *clover_gamma);

    // SINGLES energy histograms.
    Histogram1Dp energy_clover_raw[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS], energy_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS];
    Histogram1Dp energy_labr_3x8_raw[NUM_LABR_3X8_DETECTORS], energy_labr_3x8[NUM_LABR_3X8_DETECTORS];
    Histogram1Dp energy_labr_2x2_ss_raw[NUM_LABR_2X2_DETECTORS], energy_labr_2x2_ss[NUM_LABR_2X2_DETECTORS];
    Histogram1Dp energy_labr_2x2_fs_raw[NUM_LABR_2X2_DETECTORS], energy_labr_2x2_fs[NUM_LABR_2X2_DETECTORS];
    Histogram1Dp energy_dE_ring_raw[NUM_SI_DE_RING], energy_dE_ring[NUM_SI_DE_RING];
    Histogram1Dp energy_dE_sect_raw[NUM_SI_DE_SECT], energy_dE_sect[NUM_SI_DE_SECT];
    Histogram1Dp energy_E_raw[NUM_SI_E_DET], energy_E[NUM_SI_E_DET];

    // Time alignment spectra
    Histogram2Dp align_time_clover[NUM_CLOVER_DETECTORS];
    Histogram2Dp align_time_labr_3x8;
    Histogram2Dp align_time_labr_2x2_ss, align_time_labr_2x2_fs;
    Histogram2Dp align_time_de_ring, align_time_de_sect;
    Histogram2Dp align_time_e;
    Histogram2Dp time_ring_sect[NUM_SI_DE_SECT];

    // Addback spectra
    Histogram1Dp energy_clover_addback[NUM_CLOVER_DETECTORS];
    Histogram2Dp time_clover_addback[NUM_CLOVER_DETECTORS];

    // Timediff to the 'trigger'.
    Histogram2Dp prompt_de_ring, prompt_de_sect;
    Histogram2Dp prompt_labr_3x8, prompt_clover;

    // Time vs. energy E detector
    Histogram2Dp time_energy_e;

    // dE vs E
    Histogram2Dp ede_raw[NUM_SI_E_DET][NUM_SI_DE_RING], ede[NUM_SI_E_DET][NUM_SI_DE_RING];
    Histogram2Dp ede_raw_sect[NUM_SI_DE_SECT][NUM_SI_DE_RING];
    Histogram2Dp ede_all, ede_gate;

    // 'Thickness figure'
    Histogram2Dp h_thick;
    Histogram1Dp h_ex_all, h_ex[NUM_SI_DE_RING];

    // Particle - gamma-ray coincidence matrix
    Histogram2Dp alfna, alfna_bg;
    Histogram2Dp alfna_clover, alfna_clover_bg;
    Histogram2Dp alfna_veto_ppac, alfna_veto_ppac_bg;

    // Gain clover
    Parameter gain_clover;

    // Shift clover
    Parameter shift_clover;

    // Gain labr 3.5x8
    Parameter gain_labr_3x8;

    // Shift labr 3.5x8
    Parameter shift_labr_3x8;

    // Gain labr 2x2, slow signal
    Parameter gain_labr_2x2_ss;

    // Shift labr 2x2, slow signal
    Parameter shift_labr_2x2_ss;

    // Gain labr 2x2, fast signal
    Parameter gain_labr_2x2_fs;

    // Shift labr 2x2, fast signal
    Parameter shift_labr_2x2_fs;

    // Gain dE ring
    Parameter gain_dE_ring;

    // Shift dE ring
    Parameter shift_dE_ring;

    // Gain dE sector
    Parameter gain_dE_sect;

    // Shift dE ring
    Parameter shift_dE_sect;

    // Gain E
    Parameter gain_E;

    // Shift E
    Parameter shift_E;

    // Time alignment clover
    Parameter shift_time_clover;

    // Time alignment LaBr 3.5x8
    Parameter shift_time_labr_3x8;

    // Time alignment LaBr 2x2, slow signal
    Parameter shift_time_labr_2x2_ss;

    // Time alignment LaBr 2x2, fast signal
    Parameter shift_time_labr_2x2_fs;

    // Time alignment dE
    Parameter shift_time_de_ring;

    // Time alignment dE
    Parameter shift_time_de_sect;

    // Time alignment E
    Parameter shift_time_e;

    // Coefficients of 2nd order Polynomial to calculate excitation energy from SiRi energy (E+dE).
    Parameter ex_from_ede;

    // Apparent thickness gate SiRi
    Parameter thick_range;

    // Time gates for the LaBr 3.5x8 detectors, e.g. for making the ALFNA matrices
    Parameter labr_3x8_time_cuts;

    // Time gates for the CLOVER detectors, e.g. for making the ALFNA matrices
    Parameter clover_time_cuts;

    // Time gates for the LaBr 2x2 (slow) detectors, e.g. for making the ALFNA matrices
    Parameter labr_2x2_ss_time_cuts;

    // Time gates for the LaBr 2x2 (fast) detectors, e.g. for making the ALFNA matrices
    Parameter labr_2x2_fs_time_cuts;

    // Time gates for the addback algorithm
    Parameter clover_addback_cuts;

    // Time gates for the dE ring.
    Parameter particle_ring_cuts;

    // Time gates for the dE sector.
    Parameter particle_sect_cuts;

    // Time gates for the dE sectors vs rings.
    Parameter particle_sect_ring_cuts;

    // Stuff needed for the ROOT tree
    int eDet_ID, deDet_mult, labr3x8_mult, labr2x2_mult, clover_mult;
    int labr3x8_ID[MAX_WORDS_PER_DET], labr2x2_ID[MAX_WORDS_PER_DET], clover_ID[MAX_WORDS_PER_DET];
    int deDet_ringID[MAX_WORDS_PER_DET], deDet_sectID[MAX_WORDS_PER_DET];
    double eDet_energy, eDet_timestamp_fine;
    int64_t eDet_timestamp_course;
    double labr3x8_energy[MAX_WORDS_PER_DET], labr2x2_energy[MAX_WORDS_PER_DET], clover_energy[MAX_WORDS_PER_DET];
    double labr3x8_timestamp_fine[MAX_WORDS_PER_DET], labr2x2_timestamp_fine[MAX_WORDS_PER_DET], clover_timestamp_fine[MAX_WORDS_PER_DET];
    int64_t labr3x8_timestamp_coarse[MAX_WORDS_PER_DET], labr2x2_timestamp_coarse[MAX_WORDS_PER_DET], clover_timestamp_coarse[MAX_WORDS_PER_DET];
    double deDet_ring_energy[MAX_WORDS_PER_DET], deDet_sect_energy[MAX_WORDS_PER_DET];
    double deDet_ring_timestamp_fine[MAX_WORDS_PER_DET], deDet_sect_timestamp_fine[MAX_WORDS_PER_DET];
    int64_t deDet_ring_timestamp_coarse[MAX_WORDS_PER_DET], deDet_sect_timestamp_coarse[MAX_WORDS_PER_DET];
    bool is_opt;

    int64_t n_fail_de_ring, n_fail_de_sect, n_fail_e;

    int64_t n_tot_e, n_tot_de_ring, n_tot_de_sect;
    int64_t tot;


};

#endif // USERROUTINE2_H
