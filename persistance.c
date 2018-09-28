/*
 *  Off-the-Record Messaging plugin for pidgin
 *  Copyright (C) 2004-2018  Ian Goldberg, Rob Smits,
 *                           Chris Alexander, Willy Lew,
 *                           Nikita Borisov
 *                           <otr@cypherpunks.ca>
 *                           The pidgin-otrng contributors
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>

#include <glib.h>
#include <glib/gstdio.h>

#include <libotr-ng/messaging.h>

#include "persistance.h"
#include "pidgin-helpers.h"

#ifdef ENABLE_NLS
/* internationalisation header */
#include <glib/gi18n-lib.h>
#else
#define _(x) (x)
#define N_(x) (x)
#endif

int persistance_write_privkey_v4_FILEp(otrng_global_state_s *otrng_state) {
#ifndef WIN32
  mode_t mask;
#endif /* WIN32 */
  FILE *privf;

  gchar *privkeyfile =
      g_build_filename(purple_user_dir(), PRIVKEY_FILE_NAME_V4, NULL);
  if (!privkeyfile) {
    fprintf(stderr, _("Out of memory building filenames!\n"));
    return -1;
  }
#ifndef WIN32
  mask = umask(0077);
#endif /* WIN32 */
  privf = g_fopen(privkeyfile, "w+b");
#ifndef WIN32
  umask(mask);
#endif /* WIN32 */

  g_free(privkeyfile);
  if (!privf) {
    fprintf(stderr, _("Could not write private key file\n"));
    return -1;
  }

  int err = 0;
  if (otrng_failed(
          otrng_global_state_private_key_v4_write_FILEp(otrng_state, privf))) {
    err = -1;
  }
  fclose(privf);

  return err;
}

void persistance_load_private_keys_v4(otrng_global_state_s *otrng_state) {
  gchar *f = g_build_filename(purple_user_dir(), PRIVKEY_FILE_NAME_V4, NULL);
  if (!f) {
    return;
  }

  FILE *fp = g_fopen(f, "rb");
  g_free(f);

  otrng_global_state_private_key_v4_read_FILEp(
      otrng_state, fp, protocol_and_account_to_purple_conversation);

  if (fp) {
    fclose(fp);
  }
}
