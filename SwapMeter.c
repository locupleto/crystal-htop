/*
htop - SwapMeter.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPLv2+, see the COPYING file
in the source distribution for its full text.
*/

/*
 * crystal_htop - Additions
 * 2024-01-13 Urban Ottosson
 *
 * This file contains minor additions to the original htop codebase.
 * These modifications enable htop to log samples of its measurements to a file,
 * a functionality not provided in the original htop.
 *
 * All additions are clearly marked to facilitate easy comparison with
 * the original code. Search for 'crystal_htop' in the code.
 *
 * Repository: https://github.com/locupleto/crystal_htop
 */

#include "config.h" // IWYU pragma: keep

#include "SwapMeter.h"

#include <math.h>
#include <stddef.h>

#include "CRT.h"
#include "Macros.h"
#include "Object.h"
#include "Platform.h"
#include "RichString.h"


static const int SwapMeter_attributes[] = {
   SWAP,
   SWAP_CACHE,
   SWAP_FRONTSWAP,
};

static void SwapMeter_updateValues(Meter* this) {
   char* buffer = this->txtBuffer;
   size_t size = sizeof(this->txtBuffer);
   int written;

   this->values[SWAP_METER_CACHE] = NAN;   /* 'cached' not present on all platforms */
   this->values[SWAP_METER_FRONTSWAP] = NAN;   /* 'frontswap' not present on all platforms */
   Platform_setSwapValues(this);

      written = Meter_humanUnit(buffer, this->values[SWAP_METER_USED], size);
   /* --- Start of crystal_htop Additions --- */
   char swapUsedHumanBuffer[256] = { 0 };
   strncpy(swapUsedHumanBuffer, buffer, sizeof(swapUsedHumanBuffer) - 1);
   double swapUsedRaw = this->values[SWAP_METER_USED];
   /* --- End of crystal_htop Additions --- */
   METER_BUFFER_CHECK(buffer, size, written);

   METER_BUFFER_APPEND_CHR(buffer, size, '/');

   Meter_humanUnit(buffer, this->total, size);
   /* --- Start of crystal_htop Additions --- */
   char swapTotalHumanBuffer[256] = { 0 };
   strncpy(swapTotalHumanBuffer, buffer, sizeof(swapTotalHumanBuffer) - 1);
   double swapTotalRaw = this->total;

   // Determine temp directory
   const char* defaultTempDir = "/tmp";
   const char* tempDir = getenv("HTOP_TEMP_DIR") ? getenv("HTOP_TEMP_DIR") : defaultTempDir;

   // Write to swap used file
   char swapUsedFilePath[256];
   snprintf(swapUsedFilePath, sizeof(swapUsedFilePath), "%s/htop_swap_used.txt", tempDir);
   FILE *file = fopen(swapUsedFilePath, "w");
   if (file != NULL) {
      fprintf(file, "%f %s", swapUsedRaw, swapUsedHumanBuffer);
      fclose(file);
   }

   // Write to total swap file
   char swapTotalFilePath[256];
   snprintf(swapTotalFilePath, sizeof(swapTotalFilePath), "%s/htop_swap_total.txt", tempDir);
   file = fopen(swapTotalFilePath, "w");
   if (file != NULL) {
      fprintf(file, "%f %s", swapTotalRaw, swapTotalHumanBuffer);
      fclose(file);
   }
   /* --- Start of crystal_htop Additions --- */
}

static void SwapMeter_display(const Object* cast, RichString* out) {
   char buffer[50];
   const Meter* this = (const Meter*)cast;
   RichString_writeAscii(out, CRT_colors[METER_TEXT], ":");
   Meter_humanUnit(buffer, this->total, sizeof(buffer));
   RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);
   Meter_humanUnit(buffer, this->values[SWAP_METER_USED], sizeof(buffer));
   RichString_appendAscii(out, CRT_colors[METER_TEXT], " used:");
   RichString_appendAscii(out, CRT_colors[METER_VALUE], buffer);

   if (isNonnegative(this->values[SWAP_METER_CACHE])) {
      Meter_humanUnit(buffer, this->values[SWAP_METER_CACHE], sizeof(buffer));
      RichString_appendAscii(out, CRT_colors[METER_TEXT], " cache:");
      RichString_appendAscii(out, CRT_colors[SWAP_CACHE], buffer);
   }

   if (isNonnegative(this->values[SWAP_METER_FRONTSWAP])) {
      Meter_humanUnit(buffer, this->values[SWAP_METER_FRONTSWAP], sizeof(buffer));
      RichString_appendAscii(out, CRT_colors[METER_TEXT], " frontswap:");
      RichString_appendAscii(out, CRT_colors[SWAP_FRONTSWAP], buffer);
   }
}

const MeterClass SwapMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete,
      .display = SwapMeter_display,
   },
   .updateValues = SwapMeter_updateValues,
   .defaultMode = BAR_METERMODE,
   .maxItems = SWAP_METER_ITEMCOUNT,
   .total = 100.0,
   .attributes = SwapMeter_attributes,
   .name = "Swap",
   .uiName = "Swap",
   .caption = "Swp"
};
