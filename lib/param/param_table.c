/*
   Unix SMB/CIFS implementation.
   Parameter loading functions
   Copyright (C) Karl Auer 1993-1998

   Largely re-written by Andrew Tridgell, September 1994

   Copyright (C) Simo Sorce 2001
   Copyright (C) Alexander Bokovoy 2002
   Copyright (C) Stefan (metze) Metzmacher 2002
   Copyright (C) Jim McDonough <jmcd@us.ibm.com> 2003
   Copyright (C) Michael Adam 2008
   Copyright (C) Jelmer Vernooij <jelmer@samba.org> 2007
   Copyright (C) Andrew Bartlett 2011

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

static const struct enum_list enum_protocol[] = {
	{PROTOCOL_SMB2_10, "SMB2"}, /* for now keep PROTOCOL_SMB2_10 */
	{PROTOCOL_SMB3_00, "SMB3"}, /* for now keep PROTOCOL_SMB3_00 */
	{PROTOCOL_SMB3_00, "SMB3_00"},
	{PROTOCOL_SMB2_24, "SMB2_24"},
	{PROTOCOL_SMB2_22, "SMB2_22"},
	{PROTOCOL_SMB2_10, "SMB2_10"},
	{PROTOCOL_SMB2_02, "SMB2_02"},
	{PROTOCOL_NT1, "NT1"},
	{PROTOCOL_LANMAN2, "LANMAN2"},
	{PROTOCOL_LANMAN1, "LANMAN1"},
	{PROTOCOL_CORE, "CORE"},
	{PROTOCOL_COREPLUS, "COREPLUS"},
	{PROTOCOL_COREPLUS, "CORE+"},
	{-1, NULL}
};

static const struct enum_list enum_security[] = {
	{SEC_AUTO, "AUTO"},
	{SEC_USER, "USER"},
	{SEC_DOMAIN, "DOMAIN"},
#if (defined(HAVE_ADS) || _SAMBA_BUILD_ >= 4)
	{SEC_ADS, "ADS"},
#endif
	{-1, NULL}
};

static const struct enum_list enum_bool_auto[] = {
	{false, "No"},
	{false, "False"},
	{false, "0"},
	{true, "Yes"},
	{true, "True"},
	{true, "1"},
	{Auto, "Auto"},
	{-1, NULL}
};

static const struct enum_list enum_csc_policy[] = {
	{CSC_POLICY_MANUAL, "manual"},
	{CSC_POLICY_DOCUMENTS, "documents"},
	{CSC_POLICY_PROGRAMS, "programs"},
	{CSC_POLICY_DISABLE, "disable"},
	{-1, NULL}
};

/* Server role options */
static const struct enum_list enum_server_role[] = {
	{ROLE_AUTO, "auto"},
	{ROLE_STANDALONE, "standalone server"},
	{ROLE_STANDALONE, "standalone"},
	{ROLE_DOMAIN_MEMBER, "member server"},
	{ROLE_DOMAIN_MEMBER, "member"},
	{ROLE_DOMAIN_PDC, "classic primary domain controller"},
	{ROLE_DOMAIN_BDC, "classic backup domain controller"},
	{ROLE_ACTIVE_DIRECTORY_DC, "active directory domain controller"},
	{ROLE_ACTIVE_DIRECTORY_DC, "domain controller"},
	{ROLE_ACTIVE_DIRECTORY_DC, "dc"},
	{-1, NULL}
};

/* SMB signing types. */
static const struct enum_list enum_smb_signing_vals[] = {
	{SMB_SIGNING_DEFAULT, "default"},
	{SMB_SIGNING_OFF, "No"},
	{SMB_SIGNING_OFF, "False"},
	{SMB_SIGNING_OFF, "0"},
	{SMB_SIGNING_OFF, "Off"},
	{SMB_SIGNING_OFF, "disabled"},
	{SMB_SIGNING_IF_REQUIRED, "if_required"},
	{SMB_SIGNING_IF_REQUIRED, "Yes"},
	{SMB_SIGNING_IF_REQUIRED, "True"},
	{SMB_SIGNING_IF_REQUIRED, "1"},
	{SMB_SIGNING_IF_REQUIRED, "On"},
	{SMB_SIGNING_IF_REQUIRED, "enabled"},
	{SMB_SIGNING_IF_REQUIRED, "auto"},
	{SMB_SIGNING_REQUIRED, "required"},
	{SMB_SIGNING_REQUIRED, "mandatory"},
	{SMB_SIGNING_REQUIRED, "force"},
	{SMB_SIGNING_REQUIRED, "forced"},
	{SMB_SIGNING_REQUIRED, "enforced"},
	{-1, NULL}
};

/* DNS update options. */
static const struct enum_list enum_dns_update_settings[] = {
	{DNS_UPDATE_OFF, "False"},
	{DNS_UPDATE_ON, "True"},
	{DNS_UPDATE_SIGNED, "signed"},
	{-1, NULL}
};

/*
   Do you want session setups at user level security with a invalid
   password to be rejected or allowed in as guest? WinNT rejects them
   but it can be a pain as it means "net view" needs to use a password

   You have 3 choices in the setting of map_to_guest:

   "Never" means session setups with an invalid password
   are rejected. This is the default.

   "Bad User" means session setups with an invalid password
   are rejected, unless the username does not exist, in which case it
   is treated as a guest login

   "Bad Password" means session setups with an invalid password
   are treated as a guest login

   Note that map_to_guest only has an effect in user or server
   level security.
*/

static const struct enum_list enum_map_to_guest[] = {
	{NEVER_MAP_TO_GUEST, "Never"},
	{MAP_TO_GUEST_ON_BAD_USER, "Bad User"},
	{MAP_TO_GUEST_ON_BAD_PASSWORD, "Bad Password"},
        {MAP_TO_GUEST_ON_BAD_UID, "Bad Uid"},
	{-1, NULL}
};

/* Config backend options */

static const struct enum_list enum_config_backend[] = {
	{CONFIG_BACKEND_FILE, "file"},
	{CONFIG_BACKEND_REGISTRY, "registry"},
	{-1, NULL}
};


/* ADS kerberos ticket verification options */

static const struct enum_list enum_kerberos_method[] = {
	{KERBEROS_VERIFY_SECRETS, "default"},
	{KERBEROS_VERIFY_SECRETS, "secrets only"},
	{KERBEROS_VERIFY_SYSTEM_KEYTAB, "system keytab"},
	{KERBEROS_VERIFY_DEDICATED_KEYTAB, "dedicated keytab"},
	{KERBEROS_VERIFY_SECRETS_AND_KEYTAB, "secrets and keytab"},
	{-1, NULL}
};


/* ACL compatibility options. */
static const struct enum_list enum_acl_compat_vals[] = {
    { ACL_COMPAT_AUTO, "auto" },
    { ACL_COMPAT_WINNT, "winnt" },
    { ACL_COMPAT_WIN2K, "win2k" },
    { -1, NULL}
};


static const struct enum_list enum_printing[] = {
	{PRINT_SYSV, "sysv"},
	{PRINT_AIX, "aix"},
	{PRINT_HPUX, "hpux"},
	{PRINT_BSD, "bsd"},
	{PRINT_QNX, "qnx"},
	{PRINT_PLP, "plp"},
	{PRINT_LPRNG, "lprng"},
	{PRINT_CUPS, "cups"},
	{PRINT_IPRINT, "iprint"},
	{PRINT_LPRNT, "nt"},
	{PRINT_LPROS2, "os2"},
#if defined(DEVELOPER) || defined(ENABLE_BUILD_FARM_HACKS)
	{PRINT_TEST, "test"},
	{PRINT_VLP, "vlp"},
#endif /* DEVELOPER */
	{-1, NULL}
};

static const struct enum_list enum_ldap_sasl_wrapping[] = {
	{0, "plain"},
	{ADS_AUTH_SASL_SIGN, "sign"},
	{ADS_AUTH_SASL_SEAL, "seal"},
	{-1, NULL}
};

static const struct enum_list enum_ldap_ssl[] = {
	{LDAP_SSL_OFF, "no"},
	{LDAP_SSL_OFF, "off"},
	{LDAP_SSL_START_TLS, "start tls"},
	{LDAP_SSL_START_TLS, "start_tls"},
	{-1, NULL}
};

/* LDAP Dereferencing Alias types */
#define SAMBA_LDAP_DEREF_NEVER		0
#define SAMBA_LDAP_DEREF_SEARCHING	1
#define SAMBA_LDAP_DEREF_FINDING	2
#define SAMBA_LDAP_DEREF_ALWAYS		3

static const struct enum_list enum_ldap_deref[] = {
	{SAMBA_LDAP_DEREF_NEVER, "never"},
	{SAMBA_LDAP_DEREF_SEARCHING, "searching"},
	{SAMBA_LDAP_DEREF_FINDING, "finding"},
	{SAMBA_LDAP_DEREF_ALWAYS, "always"},
	{-1, "auto"}
};

static const struct enum_list enum_ldap_passwd_sync[] = {
	{LDAP_PASSWD_SYNC_OFF, "no"},
	{LDAP_PASSWD_SYNC_OFF, "off"},
	{LDAP_PASSWD_SYNC_ON, "yes"},
	{LDAP_PASSWD_SYNC_ON, "on"},
	{LDAP_PASSWD_SYNC_ONLY, "only"},
	{-1, NULL}
};

static const struct enum_list enum_map_readonly[] = {
	{MAP_READONLY_NO, "no"},
	{MAP_READONLY_NO, "false"},
	{MAP_READONLY_NO, "0"},
	{MAP_READONLY_YES, "yes"},
	{MAP_READONLY_YES, "true"},
	{MAP_READONLY_YES, "1"},
	{MAP_READONLY_PERMISSIONS, "permissions"},
	{MAP_READONLY_PERMISSIONS, "perms"},
	{-1, NULL}
};

static const struct enum_list enum_case[] = {
	{CASE_LOWER, "lower"},
	{CASE_UPPER, "upper"},
	{-1, NULL}
};