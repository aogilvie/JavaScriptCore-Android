/*
 * Copyright (c) 2003 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Copyright (c) 1999-2003 Apple Computer, Inc.  All Rights Reserved.
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*	CFUtilities.h
	Copyright (c) 1998-2003, Apple, Inc. All rights reserved.
*/

#if !defined(__COREFOUNDATION_CFUTILITIES__)
#define __COREFOUNDATION_CFUTILITIES__ 1

#include <CoreFoundation/CFBase.h>

#if defined(__cplusplus)
extern "C" {
#endif

CF_EXPORT uint32_t CFLog2(uint64_t x);

CF_EXPORT void CFMergeSortArray(void *list, CFIndex count, CFIndex elementSize, CFComparatorFunction comparator, void *context);
CF_EXPORT void CFQSortArray(void *list, CFIndex count, CFIndex elementSize, CFComparatorFunction comparator, void *context);

#if defined(__MACH__)
typedef struct {
    uint16_t	primaryVersion;
    uint8_t	secondaryVersion;
    uint8_t	tertiaryVersion;
} CFLibraryVersion;

CF_EXPORT CFLibraryVersion CFGetExecutableLinkedLibraryVersion(CFStringRef libraryName);
CF_EXPORT CFLibraryVersion CFGetExecutingLibraryVersion(CFStringRef libraryName);

/* _CFExecutableLinkedOnOrAfter(releaseVersionName) will return YES if the current executable seems to be linked on or after the specified release. Example: If you specify CFSystemVersionPuma (10.1), you will get back true for executables linked on Puma or Jaguar(10.2), but false for those linked on Cheetah (10.0) or any of its software updates (10.0.x). You will also get back false for any app whose version info could not be figured out.
    This function caches its results, so no need to cache at call sites.
*/
#endif /* __MACH__ */

typedef enum {
    CFSystemVersionCheetah = 0,		/* 10.0 */
    CFSystemVersionPuma = 1,		/* 10.1 */
    CFSystemVersionJaguar = 2,		/* 10.2? TBD */
    CFSystemVersionPanther = 3,		/* Post-Jaguar */
    CFSystemVersionPinot = 3,		/* Deprecated name for Panther */
    CFSystemVersionMerlot = 4,		/* Post-Panther */
    CFSystemVersionMax			/* This should bump up when new entries are added */
} CFSystemVersion;

#if defined(__MACH__)
CF_EXPORT Boolean _CFExecutableLinkedOnOrAfter(CFSystemVersion version);
#else
#define _CFExecutableLinkedOnOrAfter(a) (1)
#endif


#if defined(__cplusplus)
}
#endif

#endif /* ! __COREFOUNDATION_CFUTILITIES__ */

