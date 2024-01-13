/*
htop - UptimeMeter.c
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

#include "UptimeMeter.h"

#include "CRT.h"
#include "Object.h"
#include "Platform.h"
#include "XUtils.h"


static const int UptimeMeter_attributes[] = {
   UPTIME
};

static void UptimeMeter_updateValues(Meter* this) {
   int totalseconds = Platform_getUptime();
   if (totalseconds <= 0) {
      xSnprintf(this->txtBuffer, sizeof(this->txtBuffer), "(unknown)");
      return;
   }
   int seconds = totalseconds % 60;
   int minutes = (totalseconds / 60) % 60;
   int hours = (totalseconds / 3600) % 24;
   int days = (totalseconds / 86400);
   this->values[0] = days;
   if (days > this->total) {
      this->total = days;
   }
   char daysbuf[32];
   if (days > 100) {
      xSnprintf(daysbuf, sizeof(daysbuf), "%d days(!), ", days);
   } else if (days > 1) {
      xSnprintf(daysbuf, sizeof(daysbuf), "%d days, ", days);
   } else if (days == 1) {
      xSnprintf(daysbuf, sizeof(daysbuf), "1 day, ");
   } else {
      daysbuf[0] = '\0';
   }
   xSnprintf(this->txtBuffer, sizeof(this->txtBuffer), "%s%02d:%02d:%02d", daysbuf, hours, minutes, seconds);
 
   /* --- Start of crystal_htop Additions --- */
   const char* defaultTempDir = "/tmp";
   const char* tempDir;

   // Attempt to retrieve the value of the HTOP_TEMP_DIR environment variable
   const char* envTempDir = getenv("HTOP_TEMP_DIR");

   // Check if the environment variable is set
   if (envTempDir != NULL) 
      tempDir = envTempDir;
   else
      tempDir = defaultTempDir;

   char uptimeFilePath[256]; 
   snprintf(uptimeFilePath, sizeof(uptimeFilePath), "%s/htop_uptime_.txt", tempDir);
   FILE *file = fopen(uptimeFilePath, "w"); 

   if (file != NULL) {
      fprintf(file, "%s", this->txtBuffer);
      fclose(file);  
   }
   /* --- End of crystal_htop Additions --- */
}

const MeterClass UptimeMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete
   },
   .updateValues = UptimeMeter_updateValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = 1,
   .total = 100.0,
   .attributes = UptimeMeter_attributes,
   .name = "Uptime",
   .uiName = "Uptime",
   .caption = "Uptime: "
};
