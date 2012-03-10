/*****************************************************************************/
/*                                                                           */
/*                                   asm.c                                   */
/*                                                                           */
/*          Assembler output for the sp65 sprite and bitmap utility          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <errno.h>
#include <stdio.h>
#include <string.h>

/* common */
#include "cmdline.h"
#include "version.h"

/* sp65 */
#include "attr.h"
#include "bin.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void WriteAsmFile (const StrBuf* Data, const Collection* A)
/* Write the contents of Data to the given file in assembler (ca65) format */
{
    FILE*       F;
    const char* D;
    unsigned    Size;
    unsigned    BytesPerLine = 16;
    char        C;


    /* Get the file name */
    const char* Name = NeedAttrVal (A, "name", "write");

    /* Check for a bytesperline attribute */
    const char* V = GetAttrVal (A, "bytesperline");
    if ((V && sscanf (V, "%u%c", &BytesPerLine, &C) != 1) || 
        (BytesPerLine < 1 || BytesPerLine > 64)) {
        Error ("Invalid value for attribute `bytesperline'");
    }

    /* Open the output file */
    F = fopen (Name, "w");
    if (F == 0) {
        Error ("Cannot open output file `%s': %s", Name, strerror (errno));
    }

    /* Write a readable header */
    fprintf (F,
             ";\n"
             "; This file was generated by %s %s\n"
             ";\n"
             "\n",
             ProgName,
             GetVersionAsString ());

    /* Write the data */
    D    = SB_GetConstBuf (Data);
    Size = SB_GetLen (Data);
    while (Size) {

        unsigned I;

        /* Output one line */
        unsigned Chunk = Size;
        if (Chunk > BytesPerLine) {
            Chunk = BytesPerLine;         
        }
        fprintf (F, "        .byte   $%02X", (*D++ & 0xFF));
        for (I = 1; I < Chunk; ++I) {
            fprintf (F, ",%02X", (*D++ & 0xFF));
        }
        fputc ('\n', F);

        /* Bump the counters */
        Size -= Chunk;
    }

    /* Add an empty line at the end */
    fputc ('\n', F);

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing output file `%s': %s", Name, strerror (errno));
    }
}


