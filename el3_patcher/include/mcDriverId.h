/*
 * Copyright (c) 2013-2015 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RTMDRVID_H_
#define RTMDRVID_H_

#define MC_DRV_VENDOR_ID_SHIFT     (16)
#define MC_DRV_VENDOR_ID_MASK      (0xFFFF << MC_DRV_VENDOR_ID_SHIFT)
#define MC_DRV_NUMBER_MASK         (0x0000FFFF)

/** MobiCore vendor IDs. */
typedef enum {
    MC_DRV_VENDOR_ID_GD   = 0 << MC_DRV_VENDOR_ID_SHIFT,
} mcDrvVendorId_t;

/** MobiCore GD driver numbers. */
typedef enum {
    MC_DRV_NUMBER_INVALID = 0,
    MC_DRV_NUMBER_CRYPTO  = 1,
    /** Last GD driver number reserved for pre-installed drivers.
     * GD driver numbers up to this constant may not be used for loadable drivers. */
    MC_DRV_NUMBER_LAST_PRE_INSTALLED = 100,
    TB_DRV_NUMBER_TUI  = 0x101,
    TB_DRV_NUMBER_TPLAY  = 0x600,
} mcDrvNumber_t;

/** MobiCore driver IDs for Trustlets. */
typedef enum {
    MC_DRV_ID_INVALID = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_INVALID,
    MC_DRV_ID_CRYPTO  = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_CRYPTO,
    /** Last GD driver ID reserved for pre-installed drivers.
     * GD driver IDs up to this constant may not be used for loadable drivers. */
    MC_DRV_ID_LAST_PRE_INSTALLED = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_LAST_PRE_INSTALLED,
    TB_DRV_ID_TUI  = MC_DRV_VENDOR_ID_GD | TB_DRV_NUMBER_TUI,
    TB_DRV_ID_TPLAY  = MC_DRV_VENDOR_ID_GD | TB_DRV_NUMBER_TPLAY,
} mcDriverId_t;

#endif /* RTMDRVID_H_ */
