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

#ifndef MC_SPID_H_
#define MC_SPID_H_

#ifdef WIN32
#define _UNUSED
#else
#define _UNUSED __attribute__((unused))
#endif

/** Service provider Identifier type. */
typedef uint32_t mcSpid_t;

/** SPID value used as free marker in root containers. */
static _UNUSED const mcSpid_t MC_SPID_FREE = 0xFFFFFFFF;

/** Reserved SPID value. */
static _UNUSED const mcSpid_t MC_SPID_RESERVED = 0;

/** SPID for system applications. */
static _UNUSED const mcSpid_t MC_SPID_SYSTEM = 0xFFFFFFFE;

/** SPID reserved for tests only */
static _UNUSED const mcSpid_t MC_SPID_RESERVED_TEST = 0xFFFFFFFD;
static _UNUSED const mcSpid_t MC_SPID_TRUSTONIC_TEST = 0x4;

/** SPID reserved for OTA development */
static _UNUSED const mcSpid_t MC_SPID_TRUSTONIC_OTA = 0x2A;

/** GP TAs - stored in the trusted storage. They all share the same */
static _UNUSED const mcSpid_t MC_SPID_GP = 0xFFFFFFFC;

/** RTM's SPID */
static _UNUSED const mcSpid_t MC_SPID_RTM = 0xFFFFFFFB;

#endif // MC_SPID_H_

