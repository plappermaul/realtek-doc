#ifndef __CONTENTGEN_H__
#define __CONTENTGEN_H__
/*
 * Fill up content with given features.
 * catchedContent: the pointer that points to the beginning of the content. 
 * contenSize: the number of bytes of catchedContent.
 * occurTimes: the number of times that occurr in the content.
 * pattern: the pointer that points to the matched pattern.
 * retLocation: the array with the locations for each occurrence.
 * isContentAlphabet: the charator scope of the content. 1 for a-z, 0 for 0x00 - 0xff.
 */
int contentGen(char* cachedContent, uint32 contentSize, uint32 occurTimes, char* pattern, uint32** retLocations, int isContentAlphabet);

#endif
