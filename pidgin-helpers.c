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

#include <account.h>
#include <glib.h>

#include "fingerprint.h"
#include "pidgin-helpers.h"

#include <libotr-ng/client.h>
#include <libotr-ng/messaging.h>

extern otrng_global_state_s *otrng_state;

otrng_client_id_s protocol_and_account_to_client_id(const char *protocol,
                                                    const char *account) {
  otrng_client_id_s result = {
      .protocol = protocol,
      .account = account,
  };
  return result;
}

otrng_client_id_s purple_account_to_client_id(const PurpleAccount *account) {
  const char *protocol = purple_account_get_protocol_id(account);
  const char *accountname =
      g_strdup(purple_normalize(account, purple_account_get_username(account)));
  return protocol_and_account_to_client_id(protocol, accountname);
}

PurpleAccount *protocol_and_account_to_purple_account(const char *protocol,
                                                      const char *accountname) {
  return purple_accounts_find(accountname, protocol);
}

PurpleAccount *client_id_to_purple_account(const otrng_client_id_s client_id) {
  return protocol_and_account_to_purple_account(client_id.protocol,
                                                client_id.account);
}

otrng_client_s *get_otrng_client(const char *protocol,
                                 const char *accountname) {
  return otrng_client_get(
      otrng_state, protocol_and_account_to_client_id(protocol, accountname));
}

// TODO: REMOVE
otrng_client_s *purple_account_to_otrng_client(const PurpleAccount *account) {
  otrng_client_s *client =
      otrng_client_get(otrng_state, purple_account_to_client_id(account));

  /* You can set some configurations here */
  // otrng_client_set_padding(256, client);

  return client;
}

otrng_conversation_s *
purple_conversation_to_otrng_conversation(const PurpleConversation *conv) {
  PurpleAccount *account = NULL;
  char *recipient = NULL;

  account = purple_conversation_get_account(conv);
  recipient =
      g_strdup(purple_normalize(account, purple_conversation_get_name(conv)));

  otrng_client_s *client =
      otrng_client_get(otrng_state, purple_account_to_client_id(account));

  // TODO: should we force creation here?
  otrng_conversation_s *result =
      otrng_client_get_conversation(0, recipient, client);
  free(recipient);
  return result;
}

otrng_client_id_s protocol_and_account_to_purple_conversation(FILE *privf) {
  char *line = NULL;
  size_t cap = 0;
  int len = 0;

  otrng_client_id_s null_result = {
      .protocol = NULL,
      .account = NULL,
  };

  if (!privf) {
    return null_result;
  }

  while ((len = getline(&line, &cap, privf)) != -1) {
    char *delim = strchr(line, ':');

    if (!delim) {
      return null_result;
    }
    *delim = 0;
    line[len - 1] = 0; /* \n */

    return protocol_and_account_to_client_id(line, delim + 1);
  }

  return null_result;
}
