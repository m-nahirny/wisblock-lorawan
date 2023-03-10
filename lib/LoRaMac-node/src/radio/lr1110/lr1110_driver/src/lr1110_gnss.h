/*!
 * \file      lr1110_gnss.h
 *
 * \brief     GNSS scan driver definition for LR1110
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH S.A. BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __LR1110_GNSS_H__
#define __LR1110_GNSS_H__

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include "lr1110_gnss_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/*!
 * \brief Get the size of results
 *
 * This method returns the size in bytes of the results available in
 * LR1110 result buffer.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] result_size Result size
 */
void lr1110_gnss_get_result_size( const void* radio, uint16_t* result_size );

/*!
 * \brief Read GNSS results
 *
 * The GNSS results are pushed into a buffer directly. This buffer is provided
 * by the application using the driver. It MUST be long enough to contains at
 * least result_buffer_size bytes.
 *
 * \warning No check is done on result_buffer size. If this application provided
 * buffer is too low, there will be a buffer overflow bug !
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] result_buffer Application provided buffer to be filled with
 * result
 *
 * \param [in] result_buffer_size The number of bytes to read from the LR1110.
 * result_buffer must at least contains result_buffer_size bytes.
 */
void lr1110_gnss_read_results( const void* radio, uint8_t* result_buffer, const uint16_t result_buffer_size );

/*!
 * \brief Get the time spent in signal acquisition and signal analyzis
 *
 * These timings allow to compute the current consumption of the last GNSS
 * scan.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] timings GNSS timings of last GNSS scan
 */
void lr1110_gnss_get_timings( const void* radio, lr1110_gnss_timings_t* timings );

/*!
 * \brief Update almanac for one satellite
 *
 * This function is to be used to update a single satellite almanac. Note that
 * all 128 satellite almanacs must be update in a row. Therefore, this function
 * must be called 128 times in a row without any other calls in between.
 *
 * On the contrary, lr1110_gnss_almanac_full_update can be used to update all
 * almanacs in one call, but the application must be able to provide a buffer
 * that holds all almanac (>2kB).
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] almanac_bytestream Almanac buffer to update one satellite almanac
 * of the LR1110. It is up to the application to ensure that bytestream is at
 * least LR1110_GNSS_SINGLE_ALMANAC_WRITE_SIZE long.
 */
void lr1110_gnss_one_satellite_almanac_update(
    const void* radio, const lr1110_gnss_almanac_single_satellite_update_bytestram_t bytestream );

/*!
 * \brief Update full almanac for all satellites
 *
 * This function does a job similar to
 * lr1110_gnss_one_satellite_almanac_full_update, but it allows to reduced the
 * number of SPI accesses for application that can afford the huge amount of
 * memory required to store the almanac_bytestream.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] almanac_bytestream Almanac buffer to update all almanac of the
 * LR1110. It is up to the application to ensure that the buffer
 * almanac_bytestream is indeed of size
 * LR1110_GNSS_FULL_ALMANAC_WRITE_BUFFER_SIZE
 */
void lr1110_gnss_almanac_full_update( const void*                                        radio,
                                      const lr1110_gnss_almanac_full_update_bytestream_t almanac_bytestream );

/*!
 * \brief Read the almanac
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] almanac_bytestream The bytestream of the almanac
 */
void lr1110_gnss_read_almanac( const void* radio, lr1110_gnss_almanac_full_read_bytestream_t almanac_bytestream );

/*!
 * \brief Get almanac age for a satellite
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] sv_id ID of the satellite corresponding the to almanac requested
 *
 * \param [out] almanac_age Almanac age in days since last GPS time overlap
 */
void lr1110_gnss_get_almanac_age_for_satellite( const void* radio, const lr1110_gnss_satellite_id_t sv_id,
                                                uint16_t* almanac_age );

/*!
 * \brief Get almanac CRC
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] almanac_crc Almanac CRC
 */
void lr1110_gnss_get_almanac_crc( const void* radio, uint32_t* almanac_crc );

/*!
 * \brief Push data received from solver to LR1110
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] payload Payload received from solver
 *
 * \param [in] payload_size Size of the payload received from solver (in bytes)
 */
void lr1110_gnss_push_solver_msg( const void* radio, const uint8_t* payload, const uint16_t payload_size );

/*!
 * \brief Activate the GNSS scan constellation
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] constellation_mask Bit mask of the constellations to use. See
 * \ref lr1110_gnss_constellation_t for the possible values
 *
 * \see lr1110_gnss_read_used_constellations
 */
void lr1110_gnss_set_constellations_to_use( const void*                            radio,
                                            const lr1110_gnss_constellation_mask_t constellation_mask );

/*!
 * \brief Read constellation used by the GNSS scanner from the almanac update
 * configuration
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] constellations_used Bit mask of the constellations used. See
 * \ref lr1110_gnss_constellation_t for the possible values
 *
 * \see lr1110_gnss_set_constellations_to_use
 */
void lr1110_gnss_read_used_constellations( const void* radio, lr1110_gnss_constellation_mask_t* constellations_used );

/*!
 * \brief Activate the almanac update
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] constellations_to_update Bit mask of the constellations to mark
 * to update. See \ref lr1110_gnss_constellation_t for the possible values
 */
void lr1110_gnss_set_almanac_update( const void*                            radio,
                                     const lr1110_gnss_constellation_mask_t constellations_to_update );

/*!
 * \brief Function to read the almanac update configuration
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] constellations_to_update Bit mask of the constellations to mark
 * to update. See \ref lr1110_gnss_constellation_t for the possible values
 */
void lr1110_gnss_read_almanac_update( const void* radio, lr1110_gnss_constellation_mask_t* constellations_to_update );

/*!
 * \brief Function to read the GNSS firmware version
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] version GNSS Firmware version currently running on the
 * chip
 */
void lr1110_gnss_read_firmware_version( const void* radio, lr1110_gnss_version_t* version );

/*!
 * \brief Function to read the supported constellation, GPS or BEIDOU other
 * constellations
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] supported_constellations Bit mask of the constellations used.
 * See \ref lr1110_gnss_constellation_t for the possible values
 */
void lr1110_gnss_read_supported_constellations( const void*                       radio,
                                                lr1110_gnss_constellation_mask_t* supported_constellations );

/*!
 * \brief Function to set the GNSS scan mode configuration
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] scan_mode  configure single or double capture (0 or 1)
 *
 * \param [out] inter_capture_delay_second The delay between consecutive
 * captures returned by the API
 *
 * \ref lr1110_gnss_scan_mode_t
 */
void lr1110_gnss_set_scan_mode( const void* radio, const lr1110_gnss_scan_mode_t scan_mode,
                                uint8_t* inter_capture_delay_second );

/*!
 * \brief Gnss scan with no assisted parameters needed
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] date The actual date of scan. Its format is the number of
 * seconds elapsed since January the 6th 1980 00:00:00 with leap seconds
 * included.
 *
 * \param [in] gnss_input_paramaters Bit mask indicating which information
 * is added in the output payload \ref lr1110_gnss_input_paramaters_e
 *
 * \param [in] nb_sat The expected number of satellite to provide. This value
 * must be in the range [0:128]
 */
void lr1110_gnss_scan_autonomous( const void* radio, const lr1110_gnss_date_t date, const uint8_t gnss_input_paramaters,
                                  const uint8_t nb_sat );

/*!
 * \brief Gnss scan with assisted parameters.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] date The actual date of scan. Its format is the number of
 * seconds elapsed since January the 6th 1980 00:00:00 with leap seconds
 * included.
 *
 * \param [in] effort_mode Effort mode \ref lr1110_gnss_search_mode_t
 *
 * \param [in] gnss_input_paramaters Bit mask indicating which information
 * is added in the output payload \ref lr1110_gnss_input_paramaters_e
 *
 * \param [in] nb_sat The expected number of satellite to provide. This value
 * must be in the range [0:128]
 */
void lr1110_gnss_scan_assisted( const void* radio, const lr1110_gnss_date_t date,
                                const lr1110_gnss_search_mode_t effort_mode, const uint8_t gnss_input_paramaters,
                                const uint8_t nb_sat );

/*!
 * \brief Launch the second GNSS scan
 *
 * \param [in] radio Radio abstraction
 */
void lr1110_gnss_scan_continuous( const void* radio );

/*!
 * \brief Function to set the assistance position.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] assistance_position, latitude 12 bits and longitude 12 bits
 * \ref lr1110_gnss_solver_assistance_position_t
 */
void lr1110_gnss_set_assistance_position( const void*                                     radio,
                                          const lr1110_gnss_solver_assistance_position_t* assistance_position );

/*!
 * \brief Function to read the assistance position.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] assistance_position, latitude 12 bits and longitude 12 bits
 * \ref lr1110_gnss_solver_assistance_position_t
 */
void lr1110_gnss_read_assistance_position( const void*                               radio,
                                           lr1110_gnss_solver_assistance_position_t* assistance_position );

/*!
 * \brief Function to set the Xtal error.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] xtal_error, value in +/-40ppm
 */
void lr1110_gnss_set_xtal_error( const void* radio, const float xtal_error_in_ppm );

/*!
 * \brief Function to read the Xtal error.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] xtal_error, value returned between +/-30ppm
 */
void lr1110_gnss_read_xtal_error( const void* radio, float* xtal_error_in_ppm );

/*!
 * \brief Host receives an update from the network or assembles
 * itself the update message and send it to the LR1110.
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] dmc_msg buffer containing the update the network
 *
 * \param [in] dmc_msg_len len of this buffer
 */
void lr1110_gnss_push_dmc_msg( const void* radio, uint8_t* dmc_msg, uint16_t dmc_msg_len );

/*!
 * \brief Get the number of detected satellites during last scan
 *
 * \param [in] radio Radio abstraction
 *
 * \param [out] nb_detected_satellites Number of satellites detected
 */
void lr1110_gnss_get_nb_detected_satellites( const void* radio, uint8_t* nb_detected_satellites );

/*!
 * \brief Get the satellites detected on last scan with their IDs and C/N (aka.
 * CNR)
 *
 * \param [in] radio Radio abstraction
 *
 * \param [in] nb_detected_satellites Number of detected satellites on last
 * scan (obtained by calling lr1110_gnss_get_nb_detected_satellites)
 *
 * \param [out] detected_satellite_id_snr Pointer to an array of structures of
 * size big enough to contain nb_detected_satellites elements
 */
void lr1110_gnss_get_detected_satellites( const void* radio, const uint8_t nb_detected_satellites,
                                          lr1110_gnss_detected_satellite_t* detected_satellite_id_snr );
#ifdef __cplusplus
}
#endif

#endif  // __LR1110_GNSS_H__

/* --- EOF ------------------------------------------------------------------ */
