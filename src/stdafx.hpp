#include "targetver.h"
#include <shlwapi.h>
#include "MBlakerApp.hpp"
#include "MPrintHelperEx.hpp"
#include "MScanImagesDlg.hpp"
#include "MChooseOptionDlg.hpp"
#include "MScanningMovieDlg.hpp"
#include <algorithm>
#include <string>
#include <cstdio>
#include <strsafe.h>

#include "base64.hpp"
#include "Base64Int.hpp"
#include "katahiromz_pdfium.h"
#include "Renderer.hpp"
#include "crc32.hpp"
#include "mstr.hpp"
#include "fp_wrap.hpp"
#include "TextToText.hpp"
#include "MDropSource.hpp"
#include "DeleteDirectory.h"

#include "resource.h"

#define MAX_PAGES       99
#define MAX_PARTS       999

#define MAX_PRINTABLE_BYTES     (256 * 1024)    // 256KiB
#define MAX_SCREENABLE_BYTES    (512 * 1024)    // 512KiB
#define SCAN_DPI 150
